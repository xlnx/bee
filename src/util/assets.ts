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
	static importObjAsync(filename: string, callback: (e: any) => void): any {

	}
	static importJsonAsync(filename: string, callback: (e: any) => void): any {
		xhr.getAsync(filename, "text", (err: any, data: any) => {
			if (err) {
				throw "failed to import asset due to previous errors: " + err;
			}
			callback(JSON.parse(data));
		});
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
	static importAsync(filename: string, callback: (e: any) => void) {
		let format = filename.substring(filename.lastIndexOf('.') + 1);
		format = format.toLowerCase();
		switch (format) {
			case "json": return assets.importJsonAsync(filename, callback);
			case "obj": return assets.importObjAsync(filename, callback);
			default: throw "unknown model format: " + format;
		}
	}
}
