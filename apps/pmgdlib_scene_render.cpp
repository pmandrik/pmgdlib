#include <cxxopts.hpp>

#include <pmgdlib_msg.h>
#include <pmgdlib_factory.h>

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
  if(result.count("help") or not result.count("config")){
    std::cout << options.help() << std::endl;
    exit(0);
  }

  /// Get config from args.
  std::string cfg_path = result["config"].as<std::string>();

  /// Read cfg txt.
  SysOptions bo;
  bo.io = "SDL";
  Backend bk = get_backend(bo);
  std::string cfg_raw = bk.io->ReadTxt(cfg_path);

  /// Load cfg from xml.
  ConfigLoader cl;
  Config cfg = cl.LoadXmlCfg(cfg_raw);

  /// Parce scene definition, load images, load shaders, construct scene objects.
  // shared_resource_->window = backend.factory->CreateWindow(options);
  //SceneDataLoader loader(bk);
  //int ret = loader.Load(cfg);
  //if(ret != PM_SUCCESS) return ret;

  /// Render Scene: do a 1 shot run, render scene into one picture or capture the movie ..
  /// .. or keep running to let edit the scene and update XML cfg
  // SceneRender->Run();

  return 0;
}
