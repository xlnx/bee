class ulist_elem<T> {
	constructor(private elem: any, private list: ulist_base<any>, private idx: number)
	{
	}
	get(): T {
		return this.list.data[this.idx].elem;
	}
	set(value: T) {
		this.list.data[this.idx].elem = value;
	}
	remove() {
		this.list.data[this.idx] = this.list.data[this.list.data.length - 1];
		this.list.data[this.idx].idx = this.idx;
		this.list.data.pop();
	}
	translate<U>(): ulist_elem<U> {
		return new ulist_elem<U>(this.elem, this.list, this.idx);
	}
	get index(): number {
		return this.idx;
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
		return this.data.data[this.data.data.length - 1];
	}
	size(): number { 
		return this.data.data.length;
	}
	visit(callback: (e: ulist_elem<T>) => void) { 
		for (let elem of this.data.data) {
			callback(elem);
		}
	}
	subscribe(index: number): ulist_elem<T> {
		return this.data.data[index];
	}
}

export {
	ulist_elem,
	ulist
}
