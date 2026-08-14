/*
 * CAN byte sweep template.
 *
 * Sends a frame while changing one payload byte from startValue through
 * endValue, inclusive, in step-sized increments.
 *
 * intervalMs is read when the scenario starts; Restart after changing it.
 * Other Public Variables can be adjusted while the sweep is running.
 */

// @public bus = 0
// @public id = 0x321
// @public intervalMs = 100
// @public byteIndex = 0
// @public startValue = 0x00
// @public endValue = 0xFF
// @public step = 1
// @public dataText = B4 5A 14 51 00 00 25 05

var bus = 0;
var id = 0x321;
var intervalMs = 100;

var byteIndex = 0;
var startValue = 0x00;
var endValue = 0xFF;
var step = 1;

var dataText = "B4 5A 14 51 00 00 25 05";

var currentValue = 0;
var sentCount = 0;
var sweepTask = 0;

function numberValue(value) {
    const result = Number(value);
    return isFinite(result) ? result : NaN;
}

function parseHexBytes(text) {
    const tokens = String(text).trim().split(/[\s,]+/);
    const bytes = [];

    for (let i = 0; i < tokens.length; i++) {
        if (tokens[i].length === 0) {
            continue;
        }

        const value = parseInt(tokens[i], 16);

        if (isNaN(value) || value < 0 || value > 0xFF) {
            return null;
        }

        bytes.push(value);
    }

    return bytes;
}

function stopSweep(message) {
    host.cancelTask(sweepTask);
    host.log(message);
    host.requestStop();
}

function sendSweepFrame() {
    const currentBus = numberValue(bus);
    const currentId = numberValue(id);
    const currentByteIndex = numberValue(byteIndex);
    const currentEndValue = numberValue(endValue);
    const currentStep = numberValue(step);
    const payload = parseHexBytes(dataText);

    if (isNaN(currentBus) || currentBus < 0 ||
        isNaN(currentId) || currentId < 0 || currentId > 0x1FFFFFFF) {
        stopSweep("Invalid sweep bus or CAN ID.");
        return;
    }

    if (payload === null || payload.length === 0 || payload.length > 64) {
        stopSweep("dataText must contain valid hexadecimal bytes.");
        return;
    }

    if (isNaN(currentByteIndex) ||
        currentByteIndex < 0 ||
        currentByteIndex >= payload.length) {
        stopSweep(
            "byteIndex must be between 0 and " +
            (payload.length - 1) +
            "."
        );

        return;
    }

    if (isNaN(currentEndValue) ||
        currentEndValue < 0 ||
        currentEndValue > 0xFF) {
        stopSweep("endValue must be between 0x00 and 0xFF.");
        return;
    }

    if (isNaN(currentStep) || currentStep <= 0) {
        stopSweep("step must be greater than zero.");
        return;
    }

    if (currentValue > currentEndValue) {
        stopSweep(
            "Byte sweep completed after " +
            sentCount +
            " frames."
        );

        return;
    }

    payload[currentByteIndex] = currentValue & 0xFF;

    can.sendFrame(
        currentBus,
        currentId,
        payload.length,
        payload
    );

    sentCount++;

    host.log(
        "Sweep " +
        sentCount +
        ": byte[" +
        currentByteIndex +
        "] = 0x" +
        currentValue.toString(16).toUpperCase()
    );

    if (currentValue >= currentEndValue) {
        stopSweep(
            "Byte sweep completed after " +
            sentCount +
            " frames."
        );

        return;
    }

    currentValue += currentStep;
}

function setup() {
    const currentInterval = numberValue(intervalMs);
    const configuredStartValue = numberValue(startValue);
    const configuredEndValue = numberValue(endValue);

    if (isNaN(currentInterval) || currentInterval <= 0) {
        host.log("intervalMs must be greater than zero.");
        host.requestStop();
        return;
    }

    if (isNaN(configuredStartValue) ||
        configuredStartValue < 0 ||
        configuredStartValue > 0xFF ||
        isNaN(configuredEndValue) ||
        configuredEndValue < 0 ||
        configuredEndValue > 0xFF) {
        host.log("startValue and endValue must be between 0x00 and 0xFF.");
        host.requestStop();
        return;
    }

    if (configuredStartValue > configuredEndValue) {
        host.log("startValue must not be greater than endValue.");
        host.requestStop();
        return;
    }

    currentValue = configuredStartValue;
    sentCount = 0;

    sweepTask = host.scheduleEvery(
        currentInterval,
        sendSweepFrame
    );

    if (sweepTask === 0) {
        host.log("Unable to schedule byte sweep.");
        host.requestStop();
        return;
    }

    host.log(
        "Starting byte sweep from 0x" +
        configuredStartValue.toString(16).toUpperCase() +
        " through 0x" +
        configuredEndValue.toString(16).toUpperCase() +
        "."
    );
}