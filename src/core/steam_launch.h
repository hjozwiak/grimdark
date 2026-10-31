#pragma once
// The launcher's two Steam decisions, as pure functions over what gdlaunch measured about its own session
// (gdlaunch does the measuring; this decides). Engine-free, unit-tested.
//
//   decide()          "Is Steam alive for this launch?" On native Windows that is exactly what it always was: a
//                     running steam.exe. Under Wine/Proton (measured 2026-10-31) the process list only shows the
//                     Wine session's own processes, so native Linux Steam is invisible; but Proton leaves other
//                     proof behind -- the steam.exe shim process it starts for a Steam-driven launch, the app id in
//                     the environment (SteamAppId from Steam/Proton, STEAM_APPID from protontricks), the
//                     STEAM_COMPAT_DATA_PATH of this game's prefix, and the ActiveProcess registry key naming the
//                     prefix's steamclient64.dll. Any one of those (Wine only) also proves it.
//   env_fill_plan()   The environment the SteamStub wrapper wants to see. On Windows nothing sets it before
//                     gdlaunch, so it is faked wholesale, as before. Under Proton, Steam and Proton set the real
//                     values first, and overwriting SteamUser/STEAMID with placeholders would break the client's
//                     own bookkeeping -- so a variable is only ever filled when it is absent.
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace gd::core::steam_launch {

// Everything decide() is allowed to know, gathered by the launcher. Empty string = unset.
struct Probes {
  bool under_wine = false;          // ntdll exports wine_get_version (never true on native Windows)
  bool steam_exe_running = false;   // a process image named steam.exe exists in this session
  std::string env_app_id;           // first non-empty of SteamAppId, STEAM_COMPAT_APP_ID, STEAM_APPID
  std::string compat_data_path;     // STEAM_COMPAT_DATA_PATH (a Linux path ending in .../compatdata/<appid>)
  bool active_process_dll = false;  // HKCU\Software\Valve\Steam\ActiveProcess\SteamClientDll64 names an existing file
};

enum class Signal { None, SteamExe, EnvAppId, CompatDataPath, ActiveProcess };

struct Verdict {
  bool steam_alive = false;
  Signal signal = Signal::None;
  const char* reason = "";   // for the console line; what proved it (or "" when nothing did)
};

namespace detail {
// The last component of a path, over '/' or '\\', ignoring trailing separators (".../compatdata/219990/" -> "219990").
inline std::string basename(const std::string& p) {
  size_t end = p.find_last_not_of("/\\");
  if (end == std::string::npos) return {};
  size_t slash = p.find_last_of("/\\", end);
  return p.substr(slash == std::string::npos ? 0 : slash + 1, end - (slash == std::string::npos ? 0 : slash + 1) + 1);
}
}  // namespace detail

inline Verdict decide(const Probes& p, const std::string& want_app_id) {
  if (p.steam_exe_running) return {true, Signal::SteamExe, "steam.exe is running"};
  if (!p.under_wine) return {};   // native Windows: the process is the only proof, as before
  if (!p.env_app_id.empty() && p.env_app_id == want_app_id) return {true, Signal::EnvAppId, "Steam app id in the environment"};
  if (!p.compat_data_path.empty() && detail::basename(p.compat_data_path) == want_app_id)
    return {true, Signal::CompatDataPath, "this game's Proton prefix (STEAM_COMPAT_DATA_PATH)"};
  if (p.active_process_dll) return {true, Signal::ActiveProcess, "Proton prefix prepared by Steam (ActiveProcess)"};
  return {};
}

// The (name, value) pairs to set: the historical faked-Steam table, minus every variable already set to a
// non-empty value in `current`. Order matches the table gdlaunch has always used.
inline std::vector<std::pair<std::string, std::string>> env_fill_plan(const std::map<std::string, std::string>& current,
                                                                      const std::string& app_id) {
  const std::pair<std::string, std::string> table[] = {
      {"SteamEnv", "1"},        {"SteamClientLaunch", "1"},
      {"SteamGameId", app_id},  {"SteamAppId", app_id},        {"SteamOverlayGameId", app_id},
      {"SteamAppUser", "username"}, {"SteamUser", "username"}, {"STEAMID", "00000000000000000"},
      {"SESSIONNAME", "Console"},
  };
  std::vector<std::pair<std::string, std::string>> out;
  for (const auto& kv : table) {
    auto it = current.find(kv.first);
    if (it == current.end() || it->second.empty()) out.push_back(kv);
  }
  return out;
}

}  // namespace gd::core::steam_launch
