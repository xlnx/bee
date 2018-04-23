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
	private static modelPath = "./assets/"
	private static models: { [key: string]: Model } = {};

	private mesh: { [key: number]: { material: Material, mesh: Mesh[] } } = {};

	constructor(name: string) {
		name = Model.modelPath + name;
		let scene = assets.import(name);
		for (let m of scene.meshes) {
			if (!(m.materialindex in this.mesh)) {
				this.mesh[m.materialindex] = {
					material: new Material(scene.materials[m.materialindex], Model.modelPath),
					mesh: []
				};
			}
			this.mesh[m.materialindex].mesh.push(Model.createMesh(m));
		}
	}

	render() {
		for (let i in this.mesh) {
			this.mesh[i].material.use();
			for (let m of this.mesh[i].mesh) {
				m.render();
			}
		}
	}

	static create(name: string) {
		if (!(name in Model.models)) {
			Model.models[name] = new Model(name);
		}
		return Model.models[name];
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
