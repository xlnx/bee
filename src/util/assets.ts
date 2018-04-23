import xhr from "../util/xhr"

export default class assets {
	static importObj(filename: string): any {

	}
	static importJson(filename: string): any {
		try {
			let src = xhr.getSync(filename);
			return JSON.parse(src);
		} catch (e) {
			throw "failed to import asset due to previous errors: " + e;
		}
	}
	static import(filename: string): any {
		let format = filename.substring(filename.lastIndexOf('.') + 1);
		format = format.toLowerCase();
		switch (format) {
			case "json": return assets.importJson(filename);
			case "obj": return assets.importObj(filename);
			default: throw "unknown model format: " + format;
		}
	}
}
