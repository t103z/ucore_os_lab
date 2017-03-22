osascript &>/dev/null <<EOF
  tell application "iTerm2"
    activate
    tell current window to set tb to create tab with default profile
    tell current session of current window to write text "cd /Users/zhangxy/Documents/Study/os/ucore_github/labcodes/lab2/"
    tell current session of current window to write text "i386-elf-gdb -q -tui -x tools/gdbinit"
  end tell
EOF
