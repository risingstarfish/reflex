#include <doctest/doctest.h>

#include <reflex/cli.hpp>
#include <testutils.hpp>

#include <string>

using namespace reflex;
using namespace testutils;

namespace
{
struct[[= cli::command{"Dotfiles installer for Mac, Linux, Windows, and WSL"}]] dotfiles
{
  static constexpr cli::section header1 = {"ALL ACTIONS:"};

  static constexpr cli::section section1 = {"\nINSTALL"};
  // TODO: change to argument
  [[= cli::option{"-i,--install", "Install (or reinstall) available dotfiles."}
          .flag()]] bool install                                                            = false;
  [[= cli::option{"-u,--update", "Update from Git before installing."}.flag()]] bool update = false;
  [[= cli::option{
      "-r,--remove <module>",
      "Remove target modules <module>, semicolon-separated."}]] std::string          remove = "";
  [[= cli::option{
      "-R,--repair", "Remove orphaned symlinks and generated files, then re-link/generate."}
          .flag()]] bool                                                             repair = false;
  [[= cli::option{
      "--reset", "Remove all symlinks and generated files managed by this tool. (note: "
                 "copied/merged files will remain)"}
          .flag()]] bool                                                             reset  = false;
  [[= cli::option{
      "--uninstall", "Deletes symlinks, logs, and backups, then deletes install directory."}
          .flag()]] bool uninstall                                                          = false;

  // options
  // maintenance
  static constexpr cli::section section2 = {"\nMAINTENANCE"};
  [[= cli::option{"--clean [N]", "Clean backups and reset both log files."}]] int clean = 0;
  [[= cli::option{"--clean-logs", "Clean both log files."}.flag()]] bool clean_logs     = false;
  [[= cli::option{
      "--clean-backups [N]", "Clean backups. N: remove backup dirs older than N days. (default "
                             "when omitted: keep the latest "
                             "DOTFILES_MAX_BACKUPS dirs)"}]] int         clean_backups  = 0;

  static constexpr cli::section section3 = {"\nINFORMATION"};
  [[= cli::option{"-l,--list", "Display all available modules."}.flag()]] bool list = false;

  // install modules
  static constexpr cli::section section4 = {"\nINSTALL MODULES"};
  [[= cli::option{
      "-I,--include <module...>",
      "Install ONLY the specified modules <module>, semicolon-separated."}]] std::string include =
      "";
  [[= cli::option{
      "-x,--exclude <module...>",
      "Install all modules EXCEPT specified <module>, semicolon-separated."}]] std::string exclude =
      "";

  // behaviour
  static constexpr cli::section section5    = {"\nBEHAVIOUR"};
  [[= cli::option{"-n,--dry-run", "Print planned actions without modifying the disk."}
          .flag()]] bool        dry_run     = false;
  [[= cli::option{"-K,--autorestart", "Automatically restart shell at script end."}
          .flag()]] bool        autorestart = false;
  [[= cli::option{"--no-regen", "Do not regenerate relevant files if they already exist."}
          .flag()]] bool        no_regen    = false;
  [[= cli::option{"--no-backup", "Do not backup pre-existing copied files."}
          .flag()]] bool        no_backup   = false;
  [[= cli::option{"-U,--interactive", "Prompt for confirmation before every action/modification."}
          .flag()]] bool        interactive = false;
  [[= cli::option{"--noconfirm", "Do not prompt for any confirmation."}.flag()]] bool noconfirm =
      false;
  [[= cli::option{"-y,--yes", "Auto-accept yes to prompts. Alias to --noconfirm."}
          .flag()]] bool yes = false;
  [[= cli::option{"--force", "Overwrite existing files/links without backup."}.flag()]] bool force =
      false;

  // logging
  static constexpr cli::section section6 = {"\nLOGGING"};
  [[= cli::option{"-d,--debug", "Print debug output (useful for diagnosing)."}.flag()]] bool debug =
      false;
  [[= cli::option{"-q,--quiet", "Suppress all standard output except errors."}.flag()]] bool quiet =
      false;
  /* FIXME: enum*/ [[= cli::option{
      "--log-level <level>", "Set log verbosity <level>. Valid options are: trace, debug, info, "
                             "notice, warn, or error. (default: info)"}]] int log_level = 2;
  [[= cli::option{"--no-log", "Disable logging to stdout (still writes to file)."}
          .flag()]] bool                                                      no_log    = false;
  [[= cli::option{"--time", "Print total execution time at program finish."}.flag()]] bool time =
      false;

  //===============================================================================================================
  // environment variables
  //===============================================================================================================
  static constexpr cli::section env_section = {"\nENVIRONMENT VARIABLES"};
  static constexpr cli::section env1        = {"  Global (Always Active):"};
  [[= cli::env{"DOTFILES_LOG_DIR", "Path to store log files."}]] cli::env_var<std::filesystem::path>
      env_log_dir = {"~/.config/dotfiles.logs"};
  [[= cli::env{
      "DOTFILES_CACHE_DIR", "Path to store temporary cache."}]] cli::env_var<std::filesystem::path>
      env_cache_dir = {"~/.cache/dotfiles"};
  [[= cli::env{
      "DOTFILES_LOG", "Set to 0 or false to disable log file writing."}]] cli::env_var<bool>
      env_log = {true};
  [[= cli::env{
      "DOTFILES_LOCAL_MODS",
      "Set to 1 or true to install with uncommitted local changes."}]] cli::env_var<bool>
      env_local_mods = {false};
  [[= cli::env{
      "DOTFILES_AUTORESTART",
      "Set to 1 or true to restart shell at script finish."}]] cli::env_var<bool>
      env_autorestart = {false};
  [[= cli::env{
      "DOTFILES_MAX_BACKUPS",
      "Maximum number of backups to keep. Set to 0 to keep all."}]] cli::env_var<int>
      env_max_backups = {10};

  static constexpr cli::section env2 = {"  Windows Specific:"};
  [[= cli::env{
      "DOTFILES_IGNORE_HANDOFF",
      "Set to 1 or true to ignore the Windows Powershell notice."}]] cli::env_var<bool>
      env_ignore_handoff = {false};

  int operator()() const
  {
    return 0;
  }
};

} // namespace
