// P.~Mandrik, 2025

#ifndef TEST_CONFIG_HH
#define TEST_CONFIG_HH 1

#include "pmgdlib_config.h"

TEST(pmlib_config, config) {
  ConfigItem cfg_1;
  cfg_1.AddAttribute("art1", "val1");
  cfg_1.AddAttribute("art2", "1");
  cfg_1.AddAttribute("art3", "1.f");

  EXPECT_EQ(cfg_1.Attribute("art1"), "val1");
  EXPECT_EQ(cfg_1.AttributeI("art2"), 1);
  EXPECT_EQ(cfg_1.AttributeF("art3"), 1.f);

  for(int i = 0; i < 10; i++){
    ConfigItem data;
    data.AddAttribute("id", to_string(i));
    cfg_1.Add("data with id", data);
  }

  vector<ConfigItem> data = cfg_1.Get("data with id");
  EXPECT_EQ(data.size(), 10);

  vector<string> attrs = cfg_1.GetAttrsFromNested("data with id", "id");
  EXPECT_EQ(attrs.size(), 10);
  EXPECT_EQ(attrs[5], string("5"));

  cfg_1.Merge(data[0]);
  EXPECT_EQ(cfg_1.Attribute("art1"), "val1");
  EXPECT_EQ(cfg_1.Attribute("id"), "0");
}

#ifdef USE_TINYXML2
  TEST(pmlib_config, config_loader) {
    ConfigLoader cl;
    const std::string raw_cfg = R"(
      <!-- <sys screen_width="4800" screen_height="2700"/>  -->
      <sys screen_width="1600" screen_height="900"/> 

      <!-- textures -->
      <texture id="back" image_id="back_c_done.png" image_path="/home/me/my/project/GAME_ABOUT_FRIENDS/friends_dir/back_c_done.png"/>
      <texture id="c"    image_id="c_ready.png" image_path="/home/me/my/project/GAME_ABOUT_FRIENDS/friends_dir/c_ready.png"/>
      <texture id="c_small"    image_id="c_small.png" image_path="/home/me/my/project/GAME_ABOUT_FRIENDS/friends_dir/c_small.png"/>
      <texture id="blood"  image_id="blood.png" image_path="/home/me/my/project/GAME_ABOUT_FRIENDS/friends_dir/c_blood_data.png"/>

      <!-- shaders -->
      <shader id="shader_c_back" vert="Data/default_shaders/def.vert" frag="Data/default_shaders/shader_c_back.frag" imp="GL" kind="SLa">
        <var value="0.0000"/> <var value="0.0000"/> <var value="0.0000"/> <var value="0.00000"/> <var value="0.00000"/> 
      </shader>

      <!-- SLa -->
      <slaTD id="sla_back" texture_id="back" once="1"/>
      <slaTD id="sla_c_small"    texture_id="c_small"    />
      <slaTD id="sla_c"    texture_id="c"    />


      <slaFB id="sla_backbuff" once="1"/>
      <slaFBLoop id="sla_backloop" shader_id="shader_c_back" loops="2"/>
      <slaFB id="sla_fbuffer"/>

      <slaChain id="chain">
        <relation value="sla_back->sla_backbuff->sla_backloop->sla_fbuffer"/>
        <relation value="sla_fbuffer->screen"/>
        <relation value="sla_c->screen"/>
      </slaChain>
    )";

    Config cfg = cl.LoadXmlCfg(raw_cfg);

    EXPECT_EQ(cfg.Get("sys")[0].Attribute("screen_width"), "1600");
    EXPECT_EQ(cfg.Get("texture").size(), 4);
    EXPECT_EQ(cfg.Get("shader")[0].Attribute("kind"), "SLa");
    EXPECT_EQ(cfg.Get("shader")[0].Get("var")[0].AttributeF("value"), 0.f);
    EXPECT_EQ(cfg.Get("slaChain")[0].Get("relation").size(), 3);
  }
#endif

#endif
