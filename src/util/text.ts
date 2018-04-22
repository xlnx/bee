
export default class text {
	static getAsync(filename: string, callback: (err: any, text: string) => void);
	static getAsync(filename: string[], callback: (err: any, texts: string[]) => void);
	static getAsync(filename: any, callback: any) {
		let xhr = new XMLHttpRequest();
		xhr.responseType = "text";
		if (typeof filename == "string") {
			xhr.onload = (event: Event) => {
				callback(undefined, xhr.responseText);
			}
			xhr.open("get", filename);
			xhr.send();
		} else {
			if (filename.length) {
				let result: string[] = [];
				xhr.onload = (event: Event) => {
					filename = filename.slice(1);
					result.push(xhr.responseText);
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
	static getSync(filename: string): string {
		let xhr = new XMLHttpRequest();
		xhr.open("get", filename, false);
		xhr.send();
		return xhr.responseText;
	}
}
