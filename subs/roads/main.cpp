/*
Components:
 - grid map AxB, generator of start map
 - queue of tiles (railroads) to put on map; 4 first tiles are available + number of hiden tiles in the queue are shown
 - start houses with output railroads

Actions:
 - select tile from first 4 tiles
 - place on board connecting to existed road and optionally rotate
 - when connect two houses - get points and extra tiles to the queue
 - if queue is empty - uncconnected house railroad will be replaced with car road
 - save map, continue map

Drawers:
 - draw background
 - draw all bottom level elements (buttoms, static tiles on map)
 - draw all top level elements (tiles on motion, dynamic elements on map (trains))
 - particles system
pipeline = {background -> w, aqd -> w, aqd -> w, p -> window}
*/

#include <vector>
#include <stack>
#include <memory>

#define USE_SDL

#include "pmgdlib_msg.h"
#include "pmgdlib_math.h"
#include "pmgdlib_core.h"
#include "pmgdlib_factory.h"
#include "pmgdlib_sdl.h"

using namespace std;
using namespace pmgd;

enum TileClass {
    empty = 0,
    road = 1,
    railroad = 2,
    city_r = 3,
    city_rr = 4
};

enum TileType {
    undefined = 0,
    ud = 1,
    ul = 2,
    t = 3,
    d = 4,
    a = 5
};

enum Dir {
    N = 0,
    E = 1,
    S = 2,
    W = 3
};

class Clickable {
    public:
    bool press, hold, release;
    bool onClick(){ return press; }
};

struct Tile : public Clickable{
    TileClass c = TileClass::empty;
    TileType  t = TileType::undefined;
    int rotation = 0;

    int GetExit(int dir){
        std::vector<int> exits;
        int x = c;
        if(t == TileType::ud) exits = {x, 0, x, 0};
        if(t == TileType::ul) exits = {x, x, 0, 0};
        if(t == TileType::t)  exits = {x, x, x, 0};
        if(t == TileType::d)  exits = {x, 0, 0, 0};
        if(t == TileType::a)  exits = {x, x, x, x};
        std::rotate(exits.begin(), exits.begin() + rotation, exits.end());
        return exits.at(dir);
    }
};

class Map {
    public:
    int size_x, size_y;
    Map(int size_x_, int size_y_){
        size_x = size_x_;
        size_y = size_y_;
    }
    Tile tiles[100][100];

    void Set(int x, int y, Tile t){
        tiles[x][y] = t;
    }

    Tile & Get(int x, int y){
        return tiles[x][y];
    }

    bool IsFit(Tile & t, int x, int y){
        std::vector<int> shift = {0,0,0,1};
        std::vector<int> origi = {0,1,0,0};

        int counter = 0;
        for(int i = 0; i < 4; i++){
            std::rotate(shift.begin(), shift.begin()+1, shift.end());
            Tile & tt = Get(x + shift[1] - shift[3], y + shift[0] - shift[2]);
            if(tt.c == TileClass::empty) continue;
            int exit = tt.GetExit(i);

            std::rotate(origi.begin(), origi.begin()+1, origi.end());
            int exit_origin = tt.GetExit(origi[i]);
            if(exit == exit_origin) return false;
            counter++;
        }
        return counter > 0;
    }
};

class Generator {
    public:
    std::vector<TileType> tile_probs;
    void AddTileProb(TileType type, int prob){
        for(int i = 0; i < prob; i++)
            tile_probs.push_back(type);
    }

    Generator(){
        AddTileProb(TileType::ud, 2);
        AddTileProb(TileType::ul, 2);
        AddTileProb(TileType::t, 2);
        AddTileProb(TileType::d, 1);
    }

    std::shared_ptr<Map> GenMap(int size_x, int size_y, int n_out_roads){
        auto map = make_shared<Map>(size_x, size_y);

        // set start houses
        int cluster_size = 3;
        int n_clusters = (size_x / cluster_size) * (size_y / cluster_size);
        int n_houses = n_clusters / 3 + 1;
        std::vector<int> clusters_ids = rsample(n_clusters, n_houses);
        for(int i = 0; i < n_houses; ++i){
            int id = clusters_ids[i];
            int center = cluster_size / 2 + cluster_size % 2;

            int n_clusters_in_row = size_x / cluster_size;
            int x_id = id % n_clusters_in_row;
            int y_id = id / n_clusters_in_row;
            v2 c = cluster_size*v2(x_id, y_id) + v2(center, center);

            Tile t;
            t.c = TileClass::city_rr;
            t.rotation = rand() % 4;
            map->Set(c.x, c.y, t);
        }

        // set out roads
        std::vector<v2> coordinates;
        for(int i = 1; i < size_x-1; i++){
            coordinates.push_back(v2(i, 0));
            coordinates.push_back(v2(size_x+i, 0));
        }
        for(int i = 1; i < size_y-1; i++){
            coordinates.push_back(v2(0, i));
            coordinates.push_back(v2(0, size_y+i));
        }

        std::vector<int> v = rsample(coordinates.size(), n_out_roads);
        for(int i = 0; i < n_out_roads; i++){
            int p = v[i];
            v2 c = coordinates[p];
            Tile t;
            t.c = TileClass::railroad;
            t.t = TileType::a;
            map->Set(c.x, c.y, t);
        }

        return map;
    }

    Tile DiceTile(){
        Tile t;
        t.c = TileClass::railroad;
        t.t = tile_probs.at(rand() % tile_probs.size());
        return t;
    }
};

class Game {
    public:
    int selected_tile_index = -1;
    std::vector<Tile> open_queue;
    std::stack<Tile>  closed_stack;
    std::shared_ptr<Map> map;
    Generator gen;

    bool CanPut(Tile & t, int x, int y){
        if(selected_tile_index == -1) return false;
        if(t.c != TileClass::empty) return false;
        Tile selected_tile = open_queue[selected_tile_index];
        return map->IsFit(selected_tile, x, y);
    }

    void AddToStack(int n_elements){
        for(int i = 0; i < n_elements; ++i){
            Tile t = gen.DiceTile();
            closed_stack.push(t);
        }
    }

    void AddToQueue(int index){
        Tile t = closed_stack.top();
        closed_stack.pop();
        open_queue[index] = t;
    }

    void PlaceFromQueue(int index, int x, int y){
        Tile t = open_queue[index];
        map->Set(x, y, t);
        // update graph
        // check city connection condition
    }

    void BuildEnemyRoad(){

    }
    
    bool Tick(){
        // gui
        //if(menu.onClick()){
        //    return false;
        //} TODO

        // queue
        for(unsigned int i = 0; i < open_queue.size(); i++){
            Tile & t = open_queue[i];
            if(not t.onClick()) continue;
            selected_tile_index = i;
        }

        // rotate buttons
        if(selected_tile_index != -1){
            Tile & t = open_queue[selected_tile_index];
            // TODO
            //if(rotate_left_box.onClick()){
            //    t.rotation = (t.rotation - 1) % 4;
            //}
            //if(rotate_right_box.onClick()){
            //    t.rotation = (t.rotation + 1) % 4;
            //}
        }

        // tile map
        int offset = 1;
        for(int x = offset; x < map->size_x-offset; ++x){
            for(int y = offset; y < map->size_y-offset; ++y){
                Tile & t = map->Get(x, y);
                // get image from tile type and draw it
                
                if(not CanPut(t, x, y)) continue;
                // draw shadow over map tile

                if(not t.onClick()) continue;
                if(selected_tile_index == -1) continue;
                PlaceFromQueue(selected_tile_index, x, y);
                if(closed_stack.size() == 0){
                    BuildEnemyRoad();
                } else {
                    AddToQueue(selected_tile_index);
                }
                selected_tile_index = -1;
            }
        }

        return true;
    }
};

int main(){
    SysOptions bo;
    bo.io = "SDL";
    bo.multimedia_library = "SDL";
    Backend bk = get_backend(bo);

    bk.factory->sys_imp->verbose_lvl = pmgd::verbose::VERBOSE;

    std::shared_ptr<Window> wx = bk.factory->CreateWindow(bo);
    std::shared_ptr<WindowSDL> w = std::dynamic_pointer_cast<WindowSDL>(wx);

    SDL_Window * window = w->window;

    Game game();

    for(int i = 0; i < 100; i++){
        SDL_Delay(20);
    }
}