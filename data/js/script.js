const DOM_ELEMENTS = {
	tabs: {
		buttons: document.querySelectorAll('.tab-links'),
		contents: document.querySelectorAll('.tab-content')
	},
	buttons: {
		startStopButton: document.querySelector('#start-stop-calib'),
		setButtons: document.querySelectorAll('.set-button'),
		saveButtons: document.querySelectorAll('.save-button'),
		nextButton: document.querySelector('#next-temp-step'),
		resetButtons: document.querySelectorAll('.reset-button')
	},
	stepper: {
		steps: Array.from({length: 2}, (_, i) => ({
			step: document.querySelector(`#step-${i+1}`),
			content: document.querySelector(`#step-content-${i+1}`)
		}))
	},
	inputs: {
		values: document.querySelectorAll('.input-value'),
		raws: document.querySelectorAll('.input-raw')
	},
	outputs: {
		statusCalib: document.querySelector('#status-calib'),
		statusSensor: document.querySelectorAll('.status-sensor')
	},
	selections: {
		selectPh: document.querySelector('#ph-1-value')
	},
	lights: {
		traffic_light: document.querySelectorAll('.traffic-light')
	}
};

const state = {
	calibrationStart: false,
	calibrationData: {
		temp: {
			tempValue1: 0.0,
			tempRaw1: 0,
			tempValue2: 0.0,
			tempRaw2: 0,
		},
		ph: {
			phValue1: 0.0,
			phRaw1: 0
		}
	},
	currentStep: 1,
	intervals: {
		data: null,
		raw: null,
		stable: null
	},
	sensor: {
		temp: {
			status: false,
			timeLeft: 0,
		},
		ph: {
			status: false,
			timeLeft: 0,
		},
		timeOut: 30
	}	
};

document.addEventListener("DOMContentLoaded", initApp);

function initApp() {
	setupEventListeners();
	DOM_ELEMENTS.tabs.buttons[0].click();
	state.intervals.data = setInterval(updateSensorData, 2000);
}

function setupEventListeners() {
	// Tab navigation
	DOM_ELEMENTS.tabs.buttons.forEach(button => {
		button.addEventListener("click", handleTabNavigation);
	});
	// Calibration control
	DOM_ELEMENTS.buttons.startStopButton.addEventListener("click", handleCalibrationStartStop);
	DOM_ELEMENTS.buttons.setButtons.forEach(button => {
		button.addEventListener("click", handleSetButton);
	});
	DOM_ELEMENTS.buttons.saveButtons.forEach(button => {
		button.addEventListener("click", handleSaveButton);
	});
	DOM_ELEMENTS.buttons.nextButton.addEventListener("click", handleNextStep);
	DOM_ELEMENTS.buttons.resetButtons.forEach(button => {
		button.addEventListener("click", handleResetButton);
	})
}

function handleTabNavigation(event) {
	const targetTab = event.target.id.replace("tab-link-", "#tab-content-");
	
	DOM_ELEMENTS.tabs.contents.forEach(content => {
		content.style.display = "none";
	});

	DOM_ELEMENTS.tabs.buttons.forEach(button => {
		button.classList.remove("active");
	})

	document.querySelector(targetTab).style.display = "block";
	event.target.classList.add("active");
}

async function handleCalibrationStartStop(event) {
	const button = event.target;
	
	try {
		console.log("before: ", state.calibrationStart);
		const isCalibration = state.calibrationStart ? false : true;
		state.calibrationStart = isCalibration;
		console.log("after:", state.calibrationStart);

		button.disabled = true;
		button.textContent = "Sending...";

		updateCalibrationUI(isCalibration);
		startStopUpdateData(isCalibration);

		if (!isCalibration) {
			state.currentStep = 1;
			updateStepDisplay();
			disableButtons();
			resetSensorStatus();	
			resetRawInput();
			resetSensorState();
		}

		button.disabled = false;
		button.textContent = isCalibration ? "Stop Calibration" : "Start Calibration";
		console.log("Ende Start Stop");

	} catch (error) {
		DOM_ELEMENTS.outputs.statusCalib.textContent = error.message;
	}
}

function updateCalibrationUI(isCalibration) {
	DOM_ELEMENTS.selections.selectPh.disabled = !isCalibration;
	DOM_ELEMENTS.outputs.statusCalib.textContent = `Calibration is ${isCalibration ? 'started' : 'stopped'}`;

	DOM_ELEMENTS.inputs.values.forEach(input => {
		input.disabled = !isCalibration;
		if (!isCalibration) input.value = '';
	})
}

function resetRawInput() {
	DOM_ELEMENTS.inputs.raws.forEach(raw => {
		raw.textContent = '0000';
	})
}

function resetSensorStatus() {
	DOM_ELEMENTS.lights.traffic_light.forEach(light => {
		light.className = 'traffic-light unstable';
	});
	DOM_ELEMENTS.outputs.statusSensor.forEach(sensor => {
		sensor.textContent = 'Sensor calibration is stopped';
	})
}

function disableButtons() {
	DOM_ELEMENTS.buttons.saveButtons.forEach(button => {
		button.disabled = true;
	});
	DOM_ELEMENTS.buttons.nextButton.disabled = true;
	DOM_ELEMENTS.buttons.resetButtons.forEach(button => {
		button.disabled = true;
	})
	DOM_ELEMENTS.buttons.setButtons.forEach(button => {
		button.disabled = true;
	});
}

function resetSensorState() {
	state.sensor.temp.status = false;
	state.sensor.ph.status = false;
	state.sensor.temp.timeLeft = 0;
	state.sensor.ph.timeLeft = 0;
}

function startStopUpdateData(isCalibration) {
	if(isCalibration) {
		state.intervals.raw = setInterval(updateRawSensorData, 1000);
		state.intervals.stable = setInterval(updateStableStatus, 1000)
		clearInterval(state.intervals.data);
	} else {
		state.intervals.data = setInterval(updateSensorData, 1000);
		clearInterval(state.intervals.raw);
		clearInterval(state.intervals.stable);
	}
}

function updateStepDisplay() {
	DOM_ELEMENTS.stepper.steps.forEach(({step, content}, index ) => {
		
		const isCurrent = index + 1 === state.currentStep;
		const isCompleted = index + 1 < state.currentStep;

		step.className = `step u-flex-center ${isCurrent ? 'active' : ''} ${isCompleted ? 'completed' : ''}`;
		content.classList.toggle('active', isCurrent);
	});
}

function handleSetButton(event) {
	const button = event.target;
	const fieldPrefix = button.id.replace('set-', '');
	const [type, step] = fieldPrefix.split('-');

	const valueInput = document.querySelector(`#${fieldPrefix}-value`);
	const rawInput = document.querySelector(`#${fieldPrefix}-raw`);

	state.calibrationData[type][`${type}Value${step}`] = Number(valueInput.value);
	state.calibrationData[type][`${type}Raw${step}`] = Number(rawInput.innerHTML);

	button.disabled = true;
	valueInput.disabled = true;
	updateButtonStates(fieldPrefix);
}

function updateButtonStates(fieldPrefix) {
	const [type, step] = fieldPrefix.split('-');

	if (type === 'temp') {
		if (step === '1') {
			DOM_ELEMENTS.buttons.nextButton.disabled = false;
		}
		if (step === '2') {
			DOM_ELEMENTS.buttons.saveButtons[0].disabled = false;
			document.querySelector(`#step-2`).classList.remove(`active`);
			document.querySelector(`#step-2`).classList.add(`completed`);
		}
	}

	if (type === 'ph' && step === '1') {
		DOM_ELEMENTS.buttons.saveButtons[1].disabled = false;
	}
}

async function handleSaveButton(event) {
	const button = event.target;
	const dataType = button.id.includes('temp') ? 'temp' : 'ph';

	try {
		button.disabled = true;
		button.textContent = "Saving...";

		await sendDataToEsp(`/calibrate/${dataType}/set`, state.calibrationData[dataType]);

		button.textContent = "Data saved";
		resetInputs(dataType);
		resetSetButtons(dataType);
		state.currentStep = 1;
		updateStepDisplay();
		document.querySelector(`#reset-${dataType}`).disabled = false;
		document.querySelector(`#status-${dataType}`).textContent = 'Calibration data saved to ESP';
	
	} catch (error) {
		button.textContent = "Retry Saving";
		button.disabled = false;
		console.error("Failed to save", error);
		document.querySelector(`#status-${dataType}`).textContent = `Failed to save ${error}`;
	}
}

function resetInputs(type) {
	DOM_ELEMENTS.inputs.values.forEach(input => {
		if (input.getAttribute('id').includes(type)) {
			input.value = '';
			input.disabled = false;
		}
	});
}	
	
function resetSetButtons(type) {
	DOM_ELEMENTS.buttons.setButtons.forEach(button => {
		if (button.getAttribute('id').includes(type)) {
			button.disabled = false;
		}
	})
}

async function handleResetButton(event) {
	const button = event.target;
	const dataType = button.id.includes('temp') ? 'temp' : 'ph';

	state.calibrationData[dataType][`${dataType}Value1`] = 0.0;	
	state.calibrationData[dataType][`${dataType}Raw1`] = 0;

	if (dataType === 'temp') {
		state.calibrationData[dataType][`${dataType}Value2`] = 0.0;	
		state.calibrationData[dataType][`${dataType}Raw2`] = 0;
	}

	try {
		await fetch(`/calibrate/${dataType}/reset`)
			.then(response => response.text())
		button.disabled = true;
	} catch (error) {
		button.disabled = false;
		console.error('Failed to reset data:', error);
	}

}

function handleNextStep() {
	if (state.currentStep < 2) {
		state.currentStep++;
		updateStepDisplay();
		DOM_ELEMENTS.buttons.nextButton.disabled = true;
	}
}

async function sendDataToEsp(url, body) {
	const response = await fetch(url, {
		method: 'POST',
		headers: {'Content-Type': 'application/json'},
		body: JSON.stringify(body)
	});

	if (!response.ok) {
		throw new Error(`Error: ${response.status}`);
	}

	return response;
}

async function updateSensorData() {
	try {
		const data = await fetch('/data').then(response => response.json());
    
    	document.getElementById('temp').innerHTML = data.temp.toFixed(2);
    	document.getElementById('ph').innerHTML = data.ph.toFixed(2);

	} catch (error) {
		console.error('Failed to update sensor data:', error);
	}
}

async function updateRawSensorData() {
	try {
		const data = await fetch('/rawdata').then(response => response.json());

		document.getElementById('temp-1-raw').innerHTML = data.temp_raw;
		document.getElementById('temp-2-raw').innerHTML = data.temp_raw;
		document.getElementById('ph-1-raw').innerHTML = data.ph_raw;
	
		state.sensor.temp.status = data.temp_stable_status;
		state.sensor.ph.status = data.ph_stable_status;

	} catch (error) {
		console.error('Failed to update raw sensor data:', error);
		DOM_ELEMENTS.outputs.statusCalib.textContent = `Failed to update raw sensor data: ${error}`;
	}
}

function updateStableStatus() {
	stableCheck('temp', state.sensor.temp.status);
	stableCheck('ph', state.sensor.ph.status);
}

function stableCheck(dataType, isStable) {
	let sensor_state = timerCheck(dataType, isStable);
	
	DOM_ELEMENTS.buttons.setButtons.forEach(button => {
		if (button.getAttribute('id').includes(dataType)) {
			button.disabled = !isStable;
		}
	});	
	
	DOM_ELEMENTS.lights.traffic_light.forEach(light => {
		if (light.getAttribute('id').includes(dataType)) {
			light.className = `traffic-light ${sensor_state}`;
		}
	});

	DOM_ELEMENTS.outputs.statusSensor.forEach(sensor => {
		if (sensor.getAttribute('id').includes(dataType)) {
			sensor.textContent = `Sensor is ${sensor_state}`;
		}
	});
}

function timerCheck(dataType, isStable) {
	if (!isStable) {
		if (state.sensor[dataType]['timeLeft'] < state.sensor.timeOut) {
			state.sensor[dataType]['timeLeft']++;
			return 'stabilizing';
		}
		else {
			return 'unstable';
		}
	
	} else {
		state.sensor[dataType]['timeLeft'] = 0;
		return 'stable';
	}
}