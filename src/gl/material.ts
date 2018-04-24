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

	static keymap: { 
		[key: string]: { [key: string]: (self: Material, prop: MaterialProperty, path: string) => void }
	} = {
		mat: {
			shininess: (self: Material, prop: MaterialProperty) => {
				self.attributes.push({
					value: prop.value,
					uniform: Shader.uniform("float", "gSpecularPower")
				});
			},
			shinpercent: (self: Material, prop: MaterialProperty) => {
				self.attributes.push({
					value: prop.value,
					uniform: Shader.uniform("float", "gSpecularIntensity")
				});
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

	private textures: { [key: number]: { texture: Texture2D, uniform: Uniform } } = {};
	private attributes: { value: any, uniform: Uniform } [] = [];

	constructor(data: any, path: string = Material.materialPath) {
		for (let prop of data.properties) {
			let [ key, attr ] = prop.key.substring(1).split('.');
			if (key in Material.keymap) {
				if (attr in Material.keymap[key]) {
					Material.keymap[key][attr](this, <MaterialProperty>prop, path);
				}
			}
		}
	}

	use() {
		let i = 0;
		for (let loc in this.textures) {
			this.textures[loc].texture.invoke(i);
			this.textures[loc].uniform.set(i);
			i += 1;
		}
		for (let attr of this.attributes) {
			attr.uniform.set(attr.value);
		}
	}
}
