-- place in .config/nvim/lua/plugins
-- name it something like lsp.lua
return {
  {
    "neovim/nvim-lspconfig",
    opts = function()
      local keys = require("lazyvim.plugins.lsp.keymaps").get()

      -- disable signature help in insert mode
      keys[#keys + 1] = { "<c-k>", false, mode = "i" }
    end,
  },
}
