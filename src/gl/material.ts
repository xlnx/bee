import { Texture2D } from "./texture";
import { Uniform, Shader } from "./shader";

type MaterialProperty = {
	index: number;
	key: string;
	semantic: number;
	type: number;
	value: any;
}

const textureType = {
	0x1: "Diffuse",
	0x2: "Specular",
	0x3: "Ambient",
	0x4: "Emissive",
	0x5: "Height",
	0x6: "Normals",
	0x7: "Shininess",
	0x8: "Opacity",
	0x9: "Displacement",
	0xa: "Lightmap",
	0xb: "Reflection"
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
					self.textures[prop.semantic] = {
						texture: new Texture2D(path + <string>prop.value),
						uniform: Shader.uniform("int", "g" + textureType[prop.semantic])
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

	private textures: { [key: number]: { texture: Texture2D, uniform: Uniform } } = {};
	private attributes: { value: any, uniform: Uniform } [] = [];

	constructor(values?: { [key: string ]: any }) {
		if (values != undefined) {
			this.addProperties(values);
		}
	}
	use() {
		for (let loc in this.textures) {
			this.textures[loc].texture.bind(+loc);
			this.textures[loc].uniform.set(+loc);
		}
		for (let attr of this.attributes) {
			attr.uniform.set(attr.value);
		}
	}
	unuse() {
		for (let loc in this.textures) {
			this.textures[loc].texture.unbind(+loc);
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
