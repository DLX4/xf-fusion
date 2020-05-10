// const path = require("path");
const webpack = require("webpack");
// const AntDesignThemePlugin = require("antd-theme-webpack-plugin");
// const { createMockMiddleware } = require("umi-mock-middleware");

// const options = {
//   antDir: path.join(__dirname, "./node_modules/ant-design-vue"),
//   stylesDir: path.join(__dirname, "./src"),
//   varFile: path.join(
//     __dirname,
//     "./node_modules/ant-design-vue/lib/style/themes/default.less"
//   ),
//   mainLessFile: "",
//   themeVariables: ["@primary-color"],
//   generateOnce: false
// };

// const themePlugin = new AntDesignThemePlugin(options);
module.exports = {
  //axios域代理，解决axios跨域问题
  // baseUrl: "/",
  devServer: {
    open: true,
    host: "0.0.0.0",
    port: 8088,
    https: false,
    hotOnly: false,
    proxy: {
      "/api": {
        target: "http://localhost:8088/",
        // target: "http://10.10.10.195:8088",
        changeOrigin: true,
        ws: true,
        pathRewrite: {
          "^/api": ""
        },

      },

      "/ws": {
        target: "ws://localhost:8088/",
        // target: "http://10.10.10.195:8088",
        changeOrigin: true,
        ws: true,
        pathRewrite: {
          "^/ws": ""
        }
      }
    }
    // before: app => {}
  }
};
