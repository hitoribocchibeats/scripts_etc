export DISPLAY=:0 # Adjust according to your display setup
# export XAUTHORITY=/home/yourusername/.Xauthority  # Adjust the path if needed

# Define a temporary file to store the clipboard contents
CLIPBOARD_FILE="/tmp/clip.txt"

# Get the current clipboard content
CURRENT_CLIP=$(xclip -selection clipboard -o)

# Check if the temporary file already exists
PREVIOUS_CLIP="lsdjflsdjdslsdsfcldhvsxclh"
if [ -f "$CLIPBOARD_FILE" ]; then
    # Read the previous clipboard content
    PREVIOUS_CLIP=$(<"$CLIPBOARD_FILE")
else
    # If the file doesn't exist, create it with the current clipboard content
    echo "$CURRENT_CLIP" >"$CLIPBOARD_FILE"
    exit 0
fi

# Compare clipboard contents
if [ "$CURRENT_CLIP" == "$PREVIOUS_CLIP" ]; then
    # Content is the same, overwrite the clipboard
    echo "128lsdhfsl" | xclip -selection clipboard
else
    # Content is different, update the temporary file
    echo "$CURRENT_CLIP" >"$CLIPBOARD_FILE"
fi
