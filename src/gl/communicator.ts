import { ulist, ulist_elem } from "../util/ulist";
import { Shader, Uniform, UniformType } from "./shader";

class Communicator {
	public static counters: { [key: string]: Uniform } = {};

	protected data: { [key: string]: { data: any, uniform: Uniform } } = {};

	constructor(public readonly type: string, private array: boolean = true) {
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
				this.data[key].uniform.subscribe(index).set(this.data[key].data);
			}
			return true;
		} else {
			for (let key in this.data) {
				// console.log(key, this.data[key].data);
				this.data[key].uniform.set(this.data[key].data);
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
	protected init(uniforms: { [name: string]: { type: UniformType, init: any } }) {
		if (this.array) {
			for (let name in uniforms) {
				this.data[name] = {
					data: uniforms[name].init, 
					uniform: Shader.uniform(uniforms[name].type, "g" + this.type + "_" + name + "[]")
				};
			}
		} else {
			for (let name in uniforms) {
				this.data[name] = {
					data: uniforms[name].init, 
					uniform: Shader.uniform(uniforms[name].type, "g" + this.type)
				};
			}
		}
	}
}

class Communicators {
	static communicators: Communicators[] = [];
	private data = new ulist<Communicator>();

	invoke() {
		let map: { [key: string]: number } = {};
		this.data.visit((e: ulist_elem<Communicator>) => {
			let communicator = e.get();
			if (!(communicator.type in map)) {
				map[communicator.type] = 0;
			}
			if (communicator.invoke(map[communicator.type])) {
				map[communicator.type]++;
			}
		});
		for (let type in map) {
			Communicator.counters[type].set(map[type]);
		}
	}
	add(communicator: Communicator): ulist_elem<Communicator> {
		return this.data.push(communicator);
	}
	use() {
		Communicators.communicators.push(this);
	}
	unuse() {
		if (Communicators.communicators.length) {
			Communicators.communicators.pop();
		}
	}
	static get current(): Communicators {
		if (Communicators.communicators.length > 0) {
			return Communicators.communicators[Communicators.communicators.length - 1];
		} else {
			return null;
		}
	}
}

export {
	Communicator,
	Communicators
}
