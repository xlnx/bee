class ulist_elem<T> {
	constructor(private elem: any, private list: ulist_base<any>, private index: number)
	{
	}
	get(): T {
		return this.list.data[this.index].elem;
	}
	remove() {
		this.list.data[this.index] = this.list.data[this.list.data.length - 1];
		this.list.data[this.index].index = this.index;
		this.list.data.pop();
	}
	translate<U>(): ulist_elem<U> {
		return new ulist_elem<U>(this.elem, this.list, this.index);
	}
}

class ulist_base<T> {
	public data: ulist_elem<T>[] = [];
}

class ulist<T> {
	private data = new ulist_base<T>();

	push(elem: T): ulist_elem<T> {
		let object = new ulist_elem<T>(elem, this.data, this.data.data.length);
		this.data.data.push(object);
		return this.data[this.data.data.length - 1];
	}
	size(): number { 
		return this.data.data.length;
	}
	visit(callback: (e: ulist_elem<T>) => void) { 
		for (let elem of this.data.data) {
			callback(elem);
		}
	}
}

export {
	ulist_elem,
	ulist
}
