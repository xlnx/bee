import assets from "../util/assets";
import Material from "./material";
import Mesh from "./mesh";
import { VertexAttrs } from "./vertexAttrs";

const vertexAttrMap = {
	vertices: "pos3",
	colors: "color4",
	normals: "norm3",
	tangents: "tg3",
	bitangents: "bitg3",
	texturecoords: "tex2"
}

export default class Model {
	private static models: { [key: string]: Model } = {};

	private mesh: { [key: number]: { material: Material, mesh: Mesh[] } } = {};

	constructor(path: string, name: string);
	constructor(path: string, name: string, callback: (v: Model) => void);
	constructor(path: string, name: string, callback?: (v: Model) => void) {
		name = path + name;
		if (callback == undefined) {
			let scene = assets.import(name);
			for (let m of scene.meshes) {
				if (!(m.materialindex in this.mesh)) {
					this.mesh[m.materialindex] = {
						material: Material.createFromModel(scene.materials[m.materialindex], path),
						mesh: []
					};
				}
				this.mesh[m.materialindex].mesh.push(Model.createMesh(m));
			}
		} else {
			let self = this;
			assets.importAsync(name, (scene: any) => {
				for (let m of scene.meshes) {
					if (!(m.materialindex in self.mesh)) {
						self.mesh[m.materialindex] = {
							material: Material.createFromModel(scene.materials[m.materialindex], path),
							mesh: []
						};
					}
					self.mesh[m.materialindex].mesh.push(Model.createMesh(m));
				}
				callback(this);
			});
		}
	}

	render() {
		for (let i in this.mesh) {
			this.mesh[i].material.use();
			for (let m of this.mesh[i].mesh) {
				m.render();
			}
			this.mesh[i].material.unuse();
		}
	}

	static create(path: string, name: string);
	static create(path: string, name: string, callback: (v: Model) => void);
	static create(path: string, name: string, callback?: (v: Model) => void) {
		if (callback == undefined) {
			if (!(name in Model.models)) {
				Model.models[name] = new Model(path, name);
			}
			return Model.models[name];
		} else {
			if (!(name in Model.models)) {
				new Model(path, name, (v: Model) => {
					Model.models[name] = v;
					callback(v);
				});
			} else {
				callback(Model.models[name]);
			}
		}
	}
	static createMesh(mesh: any): Mesh {
		let attrs: any[] = [];
		for (let attr in mesh) {
			if (attr in vertexAttrMap) {
				attrs.push(vertexAttrMap[attr]);
			}
		}
		let vs = new VertexAttrs(attrs);
		for (let attr in mesh) {
			if (attr in vertexAttrMap) {
				vs.set(vertexAttrMap[attr], attr == "texturecoords" ? 
				mesh[attr][0] : mesh[attr]);
			}
		}
		let indices: number[] = [];
		for (let x of mesh.faces) {
			indices = indices.concat(x);
		}
		return new Mesh(vs, indices);
	}
}
