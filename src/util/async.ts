export default class asyncutil {
	static retrieve(N: number, get: (
			index: number, 
			collect: (data: any) => void
		) => void,
		finish: (
			data: any[]
		) => void
	) {
		let data = [];
		for (let i = 0; i != N; ++i) {
			get(i, (x: any) => {
				data[i] = x; --N > 0 || finish(data);
			});
		}
	}
}