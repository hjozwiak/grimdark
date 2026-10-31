#include <doctest/doctest.h>
#include "core/steam_launch.h"

using namespace gd::core::steam_launch;

TEST_CASE("decide: native Windows still requires a running steam.exe") {
  Probes none;
  CHECK(!decide(none, "219990").steam_alive);

  Probes proc;
  proc.steam_exe_running = true;
  Verdict v = decide(proc, "219990");
  CHECK(v.steam_alive);
  CHECK(v.signal == Signal::SteamExe);
}

TEST_CASE("decide: Proton signals do not count on native Windows") {
  Probes p;
  p.env_app_id = "219990";
  p.compat_data_path = "/home/u/.local/share/Steam/steamapps/compatdata/219990";
  p.active_process_dll = true;
  CHECK(!decide(p, "219990").steam_alive);   // under_wine false: a stray env var is not Steam
}

TEST_CASE("decide: under Wine the shim process still proves Steam") {
  Probes p;
  p.under_wine = true;
  p.steam_exe_running = true;
  Verdict v = decide(p, "219990");
  CHECK(v.steam_alive);
  CHECK(v.signal == Signal::SteamExe);
}

TEST_CASE("decide: under Wine each Proton signal alone suffices") {
  Probes base;
  base.under_wine = true;
  CHECK(!decide(base, "219990").steam_alive);

  Probes env = base;
  env.env_app_id = "219990";
  CHECK(decide(env, "219990").signal == Signal::EnvAppId);

  Probes compat = base;
  compat.compat_data_path = "/home/u/.local/share/Steam/steamapps/compatdata/219990";
  CHECK(decide(compat, "219990").signal == Signal::CompatDataPath);

  Probes compat_trailing = base;
  compat_trailing.compat_data_path = "/home/u/.local/share/Steam/steamapps/compatdata/219990/";
  CHECK(decide(compat_trailing, "219990").steam_alive);

  Probes compat_windows = base;
  compat_windows.compat_data_path = "Z:\\home\\u\\.steam\\steam\\steamapps\\compatdata\\219990";
  CHECK(decide(compat_windows, "219990").steam_alive);

  Probes active = base;
  active.active_process_dll = true;
  CHECK(decide(active, "219990").signal == Signal::ActiveProcess);
}

TEST_CASE("decide: another game's environment is not proof") {
  Probes p;
  p.under_wine = true;
  p.env_app_id = "123456";
  p.compat_data_path = "/home/u/.local/share/Steam/steamapps/compatdata/123456";
  CHECK(!decide(p, "219990").steam_alive);
}

TEST_CASE("env_fill_plan: an empty environment gets the whole historical table") {
  auto plan = env_fill_plan({}, "219990");
  CHECK(plan.size() == 9);
  CHECK(plan[0] == std::pair<std::string, std::string>{"SteamEnv", "1"});
  CHECK(plan[3] == std::pair<std::string, std::string>{"SteamAppId", "219990"});
  CHECK(plan[8] == std::pair<std::string, std::string>{"SESSIONNAME", "Console"});
}

TEST_CASE("env_fill_plan: real values from Steam/Proton are preserved, missing ones filled") {
  std::map<std::string, std::string> current = {
      {"SteamAppId", "219990"}, {"SteamUser", "sektor"}, {"STEAMID", "76561199383060788"}, {"SteamAppUser", ""}};
  auto plan = env_fill_plan(current, "219990");
  for (const auto& kv : plan) {
    CHECK(kv.first != "SteamAppId");
    CHECK(kv.first != "SteamUser");   // never clobbered with the "username" placeholder
    CHECK(kv.first != "STEAMID");
  }
  bool filled_app_user = false;
  for (const auto& kv : plan) filled_app_user |= kv.first == "SteamAppUser";   // present-but-empty is filled
  CHECK(filled_app_user);
  CHECK(plan.size() == 6);
}
