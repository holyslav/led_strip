const path = require("path");

module.exports = {
  outputDir: path.resolve(__dirname, "../data"),
  devServer: {
    proxy: {
      '/api': {
        target: 'http://esp-home.local:80',
        changeOrigin: true,
        ws: true
      }
    }
  }
}
