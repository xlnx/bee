import { gl, Renderer, gl2 } from "../renderer/renderer";
import { FBO } from "./buffer";
import { Communicator, Communicators } from "./communicator";
import { ulist_elem } from "../util/ulist";

class TextureCommunicator extends Communicator {
	constructor(name: string, channel: number) {
		super("Texture", false);
		let map: any = {}; 
		map[name] = {
			type: "int",
			init: channel
		}
		this.init(map);
	}
}

let channelLookup: boolean[] = [
	false, false, false, false, false, false, false, false
];

function getChannel(): number {
	for (let i = 0; i != channelLookup.length; ++i) {
		if (!channelLookup[i]) {
			channelLookup[i] = true;
			return i;
		}
	}
	throw "texture channels run out";
}

function restoreChannel(channel: number) {
	channelLookup[channel] = false;
}

abstract class Texture {
	public readonly handle: number;

	private static channels: { [name: string]: ulist_elem<Communicator> } = {};

	private name: string;
	private channel: number;

	constructor(public readonly type: number) {
		this.handle = <number>gl.createTexture();
	}
	use(name: string) {
		if (Texture.channels[name] == undefined) {
			this.name = name;
			this.channel = getChannel();
			Texture.channels[name] = Communicators.current.add(new TextureCommunicator(name, this.channel));
			gl.activeTexture(gl.TEXTURE0 + this.channel);
			gl.bindTexture(this.type, this.handle);
		} else {
			throw "channel " + name + " has already bound a texture";
		}
	}
	unuse() {
		if (Texture.channels[this.name] != undefined) {
			// gl.activeTexture(gl.TEXTURE0 + this.channel);
			gl.bindTexture(this.type, null);
			Texture.channels[this.name].remove();
			restoreChannel(this.channel);
			Texture.channels[this.name] = undefined;
		}
	}
	static genChannel(): number {
		return getChannel();
	}
	static restoreChannel(channel: number) {
		restoreChannel(channel);
	}
	protected bind() {
		gl.bindTexture(this.type, this.handle);
	}
	protected unbind() {
		gl.bindTexture(this.type, null);
	}
}

class Texture2D extends Texture {
	private w: number;
	private h: number;

	constructor(opts: { [key: string]: any });
	constructor(opts: { [key: string]: any }, width: number, height: number);
	constructor(filename: string);
	constructor(filename: string, usemipmap: boolean);
	constructor(first?: any, second?: any, third?: any) {
		super(gl.TEXTURE_2D);
		if (typeof first == "string") {
			let filename: string = first;
			let usemipmap: boolean = second == undefined ? true : second;
			this.bind();
				gl.texImage2D(this.type, 0, gl.RGBA, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE, new Uint8Array([0, 0, 0, 0]));
			this.unbind();
			let img = new Image();
			img.onload = () => {
				this.bind();
					gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
					gl.texImage2D(this.type, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, img);
					// gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
					// gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
					gl.texParameteri(this.type, gl.TEXTURE_WRAP_S, gl.REPEAT);
					gl.texParameteri(this.type, gl.TEXTURE_WRAP_T, gl.REPEAT);
					gl.texParameteri(this.type, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
					if (usemipmap) {
						gl.texParameteri(this.type, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_LINEAR);
						gl.generateMipmap(this.type);
					} else {
						gl.texParameteri(this.type, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
					}
				this.unbind();
				this.w = img.width; this.h = img.height;
			}
			img.src = filename;
		} else {
			let opts: { [key: string]: any } = first;
			let component: number = opts.component || gl.RGBA;
			let internalComponent = component;
			let type: number = opts.type || gl.UNSIGNED_BYTE;
			if (type == gl.FLOAT) {
				if (gl2) {
					switch (component) {
						case gl2.RED: internalComponent = gl2.R32F; break;
						case gl2.RG: internalComponent = gl2.RG32F; break;
						case gl.RGB: internalComponent = gl2.RGB32F; break;
						case gl.RGBA: internalComponent = gl2.RGBA32F; break;
						default: throw "unknown internal format.";
					}
				} else {
					throw "webgl 2.0 required.";
				}
			}
			this.w = second;
			this.h = third;
			if (second == undefined) {
				this.w = Renderer.instance.canvas.width;
				this.h = Renderer.instance.canvas.height;
			}
			this.bind();
				gl.texImage2D(this.type, 0, internalComponent, this.w, this.h, 0, component, type, null);
				// } else {
				// 	gl.texImage2D(this.type, 0, component, 1, 1, 0, component, gl.FLOAT, null);
				// }
				gl.texParameterf(this.type, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
				gl.texParameterf(this.type, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
				gl.texParameterf(this.type, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
				gl.texParameterf(this.type, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
			this.unbind();
		}
	}

	get width() {
		return this.w;
	}
	get height() {
		return this.h;
	}
}

class TextureCube extends Texture {
	constructor(opts: { [key: string]: any });
	constructor(opts: { [key: string]: any }, width: number, height: number);
	constructor(opts: { [key: string]: any }, width?: number, height?: number) {
		super(gl.TEXTURE_CUBE_MAP);
		if (width == undefined) {
			width = height = Renderer.instance.canvas.height;
		}
		this.bind();
			let component: number = opts.component || gl.RGBA;
			let type: number = opts.type || gl.UNSIGNED_BYTE;
			let internalComponent = component;
			if (type == gl.FLOAT) {
				if (gl2) {
					switch (component) {
						case gl2.RED: internalComponent = gl2.R32F; break;
						case gl2.RG: internalComponent = gl2.RG32F; break;
						case gl.RGB: internalComponent = gl2.RGB32F; break;
						case gl.RGBA: internalComponent = gl2.RGBA32F; break;
						default: throw "unknown internal format.";
					}
				} else {
					throw "webgl 2.0 required.";
				}
			}
			gl.texParameterf(this.type, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
			gl.texParameterf(this.type, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
			gl.texParameterf(this.type, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
			gl.texParameterf(this.type, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
			if (gl2) {
				gl.texParameterf(this.type, gl2.TEXTURE_WRAP_R, gl.CLAMP_TO_EDGE);
			}

			for (let i = 0; i != 6; ++i) {
				gl.texImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalComponent, 
						width, height, 0, component, type, null);
			}
		this.unbind();
	}
}

export {
	Texture,
	Texture2D,
	TextureCube
}
