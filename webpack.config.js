const webpack = require('webpack');
const path = require('path')
// const HtmlWebpackPlugin = require('html-webpack-plugin');
// const CopyWebpackPlugin = require('copy-webpack-plugin');

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
	output: {
		path: path.resolve(__dirname, 'build/dist'),
		filename: 'bundle.js'
	}
}
