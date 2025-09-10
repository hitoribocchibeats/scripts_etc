# pass a list of scripts in ~/scripts to rofi  then run the selection

export PATH=$PATH:/home/$USER/.cargo/bin:/home/$USER/odin-linux-amd64-dev-2025-07/:/home/$USER/.bun/bin:/home/$USER/scripts/:/usr/lib/emscripten:/home/$USER/.nimble/bin:/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl:/home/$USER/.local/bin:/home/$USER/go/bin:/usr/local/go/bin:/home/$USER/.local/share/applications/

selected_script=$(rofi -dmenu -p "Select a script:" < <(ls ~/scripts))

if [ -n "$selected_script" ]; then
    bash ~/scripts/"$selected_script"
else
    echo "No script selected."
fi
