const missions = {
	bismark: {
		sea: [{
			className: "bismark",
			position: [1350, 180],
			target: 0,
			speed: 0.1
		}, {
			className: "hipper",
			position: [-270, -420],
			target: -90,
			speed: 0.15
		}, {
			className: "hipper",
			position: [-320, -800],
			target: -84,
			speed: 0.15
		}, {
			className: "clemson",
			position: [-800, -320],
			target: -90,
			speed: 0.12
		}, {
			className: "clemson",
			position: [-760, -800],
			target: -84,
			speed: 0.12
		}],
		uboat: {
			className: "7c",
			target: -90,
			speed: 0,
			depth: 8
		},
		ambient: {
			time: 9
		}
	},
	narvik: {
		sea: [{
			className: "illustrious",
			position: [-1000, -1300],
			target: 180,
			speed: 0.2
		}, {
			className: "hipper",
			position: [-900, -900],
			target: 180,
			speed: 0.2
		}, {
			className: "clemson",
			position: [-100, -1150],
			target: 180,
			speed: 0.2
		}, {
			className: "clemson",
			position: [-1000, -1650],
			target: 183,
			speed: 0.2
		}, {
			className: "clemson",
			position: [-1650, -1300],
			target: 180,
			speed: 0.2
		}],
		uboat: {
			className: "7c",
			target: 110,
			speed: 0,
			depth: 8
		},
		ambient: {
			time: 12.5
		}
	},
	test: {
		sea: [{
			className: "clemson",
			position: [0, 0],
			target: 90,
			speed: 0
		}],
		uboat: {
			className: "7c",
			target: 90,
			speed: 0,
			dive: 0
		},
		ambient: {
			time: 8
		}
	},
	".narvik": {
		sea: [{
			className: "illustrious",
			position: [-100, -300],
			target: 180,
			speed: 0.2
		}, {
			className: "hipper",
			position: [0, 100],
			target: 180,
			speed: 0.2
		}, {
			className: "clemson",
			position: [800, -150],
			target: 180,
			speed: 0.2
		}, {
			className: "clemson",
			position: [-100, -650],
			target: 183,
			speed: 0.2
		}, {
			className: "clemson",
			position: [-750, -300],
			target: 180,
			speed: 0.2
		}],
		uboat: {
			className: "7c",
			target: 110,
			speed: 0,
			depth: 8
		},
		ambient: {
			time: 7
		}
	}
};

export {
	missions
}