import { Texture2D, Texture } from "./texture";
import { Uniform, Shader } from "./shader";
import { gl } from "../renderer/renderer";

type MaterialProperty = {
	index: number;
	key: string;
	semantic: number;
	type: number;
	value: any;
}

const textureType = {
	0x1: "MatDiffuse",
	0x2: "MatSpecular",
	0x3: "MatAmbient",
	0x4: "MatEmissive",
	0x5: "MatHeight",
	0x6: "MatNormals",
	0x7: "MatShininess",
	0x8: "MatOpacity",
	0x9: "MatDisplacement",
	0xa: "MatLightmap",
	0xb: "MatReflection"
}

export default class Material {
	static materialPath: string = "./materials/";

	private static keymap: { 
		[key: string]: { [key: string]: (self: Material, prop: MaterialProperty, path: string) => void }
	} = {
		mat: {
			shininess: (self: Material, prop: MaterialProperty) => {
				Material.lookup.specularPower(self, prop.value);
			},
			shinpercent: (self: Material, prop: MaterialProperty) => {
				Material.lookup.specularIntensity(self, prop.value);
			}
		},
		tex: {
			file: (self: Material, prop: MaterialProperty, path: string) => {
				if (prop.semantic in textureType) {
					console.log(textureType[prop.semantic]);
					self.textures[textureType[prop.semantic]] = {
						texture: new Texture2D(path + <string>prop.value),
						channel: undefined
					};
				}
			}
		}
	};
	private static lookup: { [key: string]: (self: Material, value: any) => void } = {
		specularPower: (self: Material, value: number) => {
			self.attributes.push({
				value: value,
				uniform: Shader.uniform("float", "gSpecularPower")
			});
		},
		specularIntensity: (self: Material, value: number) => {
			self.attributes.push({
				value: value,
				uniform: Shader.uniform("float", "gSpecularIntensity")
			});
		},
		texture: (self: Material, value: any) => {
			// 
		}
	};

	private textures: { [key: string]: { texture: Texture2D, channel: number } } = {};
	private attributes: { value: any, uniform: Uniform } [] = [];

	constructor(values?: { [key: string ]: any }) {
		if (values != undefined) {
			this.addProperties(values);
		}
	}
	use() {
		for (let name in this.textures) {
			this.textures[name].channel = Texture.genChannel();
			gl.activeTexture(gl.TEXTURE0 + this.textures[name].channel);
			gl.bindTexture(gl.TEXTURE_2D, this.textures[name].texture.handle);
			Shader.uniform("int", "g" + name).set(this.textures[name].channel);
			// console.log("g" + name, this.textures[name].channel);
			// this.textures[name].use(name);
		}
		for (let attr of this.attributes) {
			attr.uniform.set(attr.value);
		}
	}
	unuse() {
		for (let name in this.textures) {
			// gl.activeTexture(gl.TEXTURE0 + this.textures[name].channel);
			gl.bindTexture(gl.TEXTURE_2D, null);
			Texture.restoreChannel(this.textures[name].channel);
			// this.textures[name].unuse();
		}
	}
	addProperties(values: { [key: string ]: any }) {
		for (let key in values) {
			if (key in Material.lookup) {
				Material.lookup[key](this, values[key]);
			}
		}
	}

	static createFromModel(data: any, path: string = Material.materialPath) {
		let self = new Material();
		for (let prop of data.properties) {
			let [ key, attr ] = prop.key.substring(1).split('.');
			if (key in Material.keymap) {
				if (attr in Material.keymap[key]) {
					Material.keymap[key][attr](self, <MaterialProperty>prop, path);
				}
			}
		}
		return self;
	}
}
