const webpack = require('webpack');
const CopyWebpackPlugin = require('copy-webpack-plugin');
const path = require('path')
// const HtmlWebpackPlugin = require('html-webpack-plugin');

module.exports = {
	entry: './src/browser-main.ts',
	resolve: {
		extensions: [".ts", ".tsx", ".js", ".json"]
	},
	devtool: "source-map",
	module: {
		rules: [
			{ test: /\.tsx?$/, loader: "awesome-typescript-loader" },
			{ enforce: "pre", test: /\.js$/, loader: "source-map-loader" }
		]
	},
	mode: "development",
	output: {
		path: path.resolve(__dirname, 'build/dist'),
		filename: 'bundle.js'
	},
	plugins: [
		new CopyWebpackPlugin([
			{
				from: "./src/index.html",
				to: "../index.html"
			},
			{
				from: "./src/shaders",
				to: "../shaders"
			},
			{
				from: "./assets",
				to: "../assets"
			}
		])
	]
}
