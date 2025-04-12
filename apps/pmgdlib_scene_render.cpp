#include <cxxopts.hpp>

#include <pmgdlib_msg.h>

using namespace std;
using namespace pmgd;

int main(int argc, char** argv){
  cxxopts::Options options("pmSceneRender", "");
  options.add_options()
    ("v,verbose", "verbose level; 0=SILENCE,5=VERBOSE", cxxopts::value<int>()->default_value(to_string(pmgd::verbose::INFO)))
    ("config", "path to scene xml config", cxxopts::value<std::string>())
    ("h,help", "Print usage")
  ;

  auto result = options.parse(argc, argv);
  if(result.count("help") or not result.count("config") or not cfg_path.size()){
    std::cout << options.help() << std::endl;
    exit(0);
  }

  // get config
  string cfg_path = result["config"].as<std::string>();

  /// read cfg txt
  BackendOptions bo;
  bo.io = "SDL";
  Backend bk = get_backend(bo);
  std::string cfg_raw = bk.io->ReadTxt(cfg_path);

  /// load cfg from xml
  ConfigLoader cl;
  std::string raw_cfg = cl.LoadXmlCfg();
  Config cfg = cl.LoadXmlCfg(cfg_raw);

  /// Parce scene definition
  

  /// Load images
  /// Load shaders
  /// Load other scene data

  /// Construct Scene
  /// Render Scene

  /// Do a 1 shot run, render scene into one micture or capture the movie ..
  /// .. or keep running to let edit the scene and update XML cfg

  return 0;
}
