type xhrType = XMLHttpRequestResponseType;

export default class xhr {
	static getAsync(filename: string, type: xhrType, callback: (err: any, data: any) => void);
	static getAsync(filename: string[], type: xhrType, callback: (err: any, data: any[]) => void);
	static getAsync(filename: any, type: xhrType, callback: any) {
		let xhr = new XMLHttpRequest();
		xhr.responseType = type;
		if (typeof filename == "string") {
			xhr.onload = (event: Event) => {
				callback(undefined, xhr.response);
			}
			xhr.open("get", filename);
			xhr.send();
		} else {
			if (filename.length) {
				let result: string[] = [];
				xhr.onload = (event: Event) => {
					filename = filename.slice(1);
					result.push(xhr.response);
					if (filename.length) {
						xhr.open("get", filename[0]);
						xhr.send();
					} else {
						callback(undefined, result);
					}
				}
				xhr.open("get", filename[0]);
				xhr.send();
			} else {
				callback(undefined, []);
			}
		}
	}
	static getSync(filename: string): any {
		let xhr = new XMLHttpRequest();
		xhr.open("get", filename, false);
		xhr.send();
		if (xhr.status == 200) {
			return xhr.response;
		} else {
			throw "failed to load resource: " + filename + " due to " +
				xhr.statusText;
		}
	}
}
