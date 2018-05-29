import { ulist_elem, ulist } from "../util/ulist"
import { Event, EventList } from "./events"

let gl: WebGLRenderingContext
let gl2: WebGL2RenderingContext

type RendererEventType = "render" | keyof DocumentEventMap

interface RendererEvent {
	cancel: () => void;
}

class RenderEvent {
	constructor(private elem: ulist_elem<any>)
	{
	}
	cancel() {
		this.elem.remove();
	}
}

class Renderer {
	protected static context: Renderer = null;
	protected static fprevTime: number = 0;

	protected fps: number = 0;
	protected events = new EventList();
	protected renderers = new ulist<() => void>();
	public readonly canvas: HTMLCanvasElement;
	protected canvasWrapper: HTMLDivElement;
	protected animationRequest: number;
	protected ftime: number = 0;
	protected fltime: number = 0;
	protected fprevTick: number = undefined;

	public fpause: boolean = false;

	public static timescale: number = 1;

	private static exts = {}

	constructor(container: HTMLElement, fallback: boolean = false) {
		if (Renderer.context != null) {
			throw "Multiple WebGL contexts not supported.";
		} else {
			this.canvasWrapper = document.createElement("div");
			this.canvas = document.createElement("canvas");
			this.canvasWrapper.style.width = "100%";
			this.canvasWrapper.style.height = "100%";
			this.canvas.width = container.clientWidth;
			this.canvas.height = container.clientHeight;
			this.canvasWrapper.appendChild(this.canvas);
			this.canvasWrapper.style["text-align"] = "center";
			this.canvasWrapper.style["overflow"] = "hidden";
			container.appendChild(this.canvasWrapper);
			try {
				fallback = fallback || (navigator.userAgent.indexOf("Mac OS X") > 0);
				if (!fallback) {
					gl2 = <WebGL2RenderingContext>this.canvas.getContext("webgl2");// || canvas.getContext("experimental-webgl");
				}
				if (!gl2) {
					gl = <WebGLRenderingContext>this.canvas.getContext("webgl") 
						|| this.canvas.getContext("experimental-webgl");
					if (!gl) {
						throw "";
					} else {
						console.warn("webgl 2.0 is not supported on your browser, fallback to webgl 1.0.");
					}
				} else {
					gl = gl2;
				}
			} catch (e) {
				console.error("webgl is not supported on your browser. " + e);
				throw "webgl is not supported on your browser. " + e;
			}
		}
		Renderer.context = this;
	}

	static get instance(): Renderer {
		if (Renderer.context) {
			return Renderer.context;
		} else {
			throw "no current renderer.";
		}
	}
	dispatch(eventType: RendererEventType, callback: any): RendererEvent {
		if (eventType == "render") {
			return new RenderEvent(this.renderers.push(callback));
		} else {
			return this.events.addEvent(eventType, callback);
		}
	}
	start() {
		this.animationRequest = window.requestAnimationFrame(this.render.bind(this));
	}
	
	static get time(): number {
		return Renderer.instance.ftime;
	}
	static get prevTime(): number {
		return Renderer.instance.fltime;
	}
	static get dt(): number {
		return Renderer.instance.ftime - Renderer.instance.fltime;
	}

	static resetClock() {
		Renderer.instance.ftime = 0;
		Renderer.instance.fltime = 0;
	}
	static pauseClock() {
		Renderer.instance.fpause = true;
	}
	static continueClock() {
		Renderer.instance.fpause = false;
	}
	static get pause(): boolean {
		return Renderer.instance.fpause;
	}

	private render(time: number) {
		time /= 1000;
		if (this.fprevTick != undefined) {
			// console.log(Renderer.timescale);
			if (!this.fpause) {
				this.fltime = this.ftime;
				this.ftime += Renderer.timescale * (time - this.fprevTick);
			}
			this.renderers.visit((e: ulist_elem<() => void>) => {
				try {
					e.get()();
				} catch (err) {
					console.error(err);
				}
			});
			if (Math.floor(time) > Renderer.fprevTime) {
				console.log("fps:", this.fps + 1);
				Renderer.fprevTime = Math.floor(time);
				this.fps = 0;
			}
			this.fps += 1;
		}
		this.fprevTick = time;
		this.animationRequest = window.requestAnimationFrame(this.render.bind(this));
	}

	static require(exts: string[]): boolean;
	static require(exts: string): boolean;
	static require(ext: any): boolean {
		if (typeof ext == "string") {
			return !!(Renderer.exts[ext] || (Renderer.exts[ext] = gl.getExtension(ext)));
		} else {
			for (let e of ext) {
				if (!(Renderer.exts[e] || (Renderer.exts[e] = gl.getExtension(e)))) {
					return false;
				}
			}
			return true;
		}
	}
	require(exts: string[]): boolean;
	require(exts: string): boolean;
	require(ext: any): boolean {
		return Renderer.require(ext);
	}
}

export {
	gl,
	gl2,
	RendererEvent,
	Renderer
}
