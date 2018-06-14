import { ulist, ulist_elem } from "../util/ulist";
import { Shader, Uniform, UniformType } from "./shader";

class Communicator {
	public static counters: { [key: string]: Uniform } = {};

	protected data: { [key: string]: { data: any, uniform: Uniform, set?: any } } = {};

	constructor(public readonly type: string, public readonly array: boolean = true) {
		if (array) {
			if (!(type in Communicator.counters)) {
				Communicator.counters[type] = Shader.uniform("int", "g" + type + "_count");
			}
		}
	}
	
	invoke(index: number): boolean {
		if (this.array) {
			for (let key in this.data) {
				// console.log(key, this.data[key].data);
				this.data[key].uniform.subscribe(index).set("set" in 
					this.data[key] ? this.data[key].set(this.data[key].data) : 
					this.data[key].data);
			}
			return true;
		} else {
			for (let key in this.data) {
				// console.log(key, this.data[key].data);
				this.data[key].uniform.set("set" in 
					this.data[key] ? this.data[key].set(this.data[key].data) : 
					this.data[key].data);
			}
			return false;
		}
	}
	set(key: string, value: any) {
		if (key in this.data) {
			this.data[key].data = value;
		} else {
			throw "communicator key does not match any: " + key;
		}
	}
	get(key: string) {
		if (key in this.data) {
			return this.data[key].data;
		} else {
			throw "communicator key does not match any: " + key;
		}
	}
	init(uniforms: { [name: string]: { 
			type: UniformType, 
			init: any, 
			set?: (value: any) => any } 
	}) {
		if (this.array) {
			for (let name in uniforms) {
				if ("set" in uniforms[name]) {
					this.data[name] = {
						data: uniforms[name].init, 
						set: uniforms[name].set,
						uniform: Shader.uniform(uniforms[name].type, "g" + this.type + "_" + name + "[]")
					};
				} else {
					this.data[name] = {
						data: uniforms[name].init, 
						uniform: Shader.uniform(uniforms[name].type, "g" + this.type + "_" + name + "[]")
					};
				}
			}
		} else {
			for (let name in uniforms) {
				if ("set" in uniforms[name]) {
					this.data[name] = {
						data: uniforms[name].init, 
						set: uniforms[name].set,
						uniform: Shader.uniform(uniforms[name].type, "g" + name)
					};
				} else {
					this.data[name] = {
						data: uniforms[name].init, 
						uniform: Shader.uniform(uniforms[name].type, "g" + name)
					};
				}
			}
		}
	}
}

class Communicators {
	static communicators: { 
		com: Communicators, 
		check: (e: Communicator) => boolean
	} [] = [];
	private data = new ulist<Communicator>();

	invoke();
	invoke(validate: (e: Communicator) => boolean);
	invoke(validate?: any) {
		if (validate == undefined) {
			validate = () => true;
		}
		let map: { [key: string]: number } = {};
		this.data.visit((e: ulist_elem<Communicator>) => {
			let communicator = e.get();
			if (communicator.array) {
				if (validate(communicator)) {
					if (!(communicator.type in map)) {
						map[communicator.type] = 0;
					}
					if (communicator.invoke(map[communicator.type])) {
						map[communicator.type]++;
					}
				}
			} else {
				if (validate(communicator)) {
					communicator.invoke(0);
				}
			}
		});
		for (let type in map) {
			Communicator.counters[type].set(map[type]);
		}
	}
	static invoke() {
		if (Communicators.current) {
			Communicators.current.invoke(Communicators.communicators[
				Communicators.communicators.length - 1
			].check);
		}
	}
	add(communicator: Communicator): ulist_elem<Communicator> {
		return this.data.push(communicator);
	}
	use();
	use(validate: (e: Communicator) => boolean);
	use(validate?: any) {
		if (validate == undefined) {
			validate = () => true;
		}
		Communicators.communicators.push({
			com: this,
			check: validate
		});
	}
	unuse() {
		if (Communicators.communicators.length) {
			Communicators.communicators.pop();
		}
	}
	static get current(): Communicators {
		if (Communicators.communicators.length > 0) {
			return Communicators.communicators[
				Communicators.communicators.length - 1].com;
		} else {
			return null;
		}
	}
}

export {
	Communicator,
	Communicators
}
