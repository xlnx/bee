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
	private static context: Renderer = null;
	private static prevTime: number = 0;

	private fps: number = 0;
	private events = new EventList();
	private renderers = new ulist<(time: number) => void>();
	public readonly canvas: HTMLCanvasElement;
	private canvasWrapper: HTMLDivElement;
	private animationRequest: number;
	private ftime: number = 0;

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
	get time(): number {
		return this.ftime;
	}

	private render(time: number) {
		this.ftime = time / 1000;
		this.renderers.visit((e: ulist_elem<(time: number) => void>) => {
			try {
				e.get()(time);
			} catch (err) {
				console.error(err);
			}
		});
		if (Math.floor(time / 1000) > Renderer.prevTime) {
			console.log("fps:", this.fps + 1);
			Renderer.prevTime = Math.floor(time / 1000);
			this.fps = 0;
		}
		this.fps += 1;
		this.animationRequest = window.requestAnimationFrame(this.render.bind(this));
	}
}

export {
	gl,
	gl2,
	RendererEvent,
	Renderer
}
