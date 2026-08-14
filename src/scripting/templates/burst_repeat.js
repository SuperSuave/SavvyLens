/*
 * Burst / Repeat CAN transmit template.
 *
 * Sends the configured payload repeatCount times, then stops.
 * intervalMs is read when the scenario starts; Restart after changing it.
 */

// @public bus = 0
// @public id = 0x321
// @public intervalMs = 100
// @public repeatCount = 10
// @public dataText = B4 5A 14 51 00 00 25 05

var bus = 0;
var id = 0x321;
var intervalMs = 100;
var repeatCount = 10;

var dataText = "B4 5A 14 51 00 00 25 05";

var sentCount = 0;
var burstTask = 0;

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

function sendBurstFrame() {
    const currentBus = numberValue(bus);
    const currentId = numberValue(id);
    const currentRepeatCount = numberValue(repeatCount);
    const data = parseHexBytes(dataText);

    if (isNaN(currentBus) || currentBus < 0 ||
        isNaN(currentId) || currentId < 0 || currentId > 0x1FFFFFFF ||
        isNaN(currentRepeatCount) || currentRepeatCount <= 0) {
        host.log("Invalid burst bus, ID, or repeat count.");
        host.cancelTask(burstTask);
        host.requestStop();
        return;
    }

    if (data === null || data.length === 0 || data.length > 64) {
        host.log("dataText must contain valid hexadecimal bytes.");
        host.cancelTask(burstTask);
        host.requestStop();
        return;
    }

    can.sendFrame(
        currentBus,
        currentId,
        data.length,
        data
    );

    sentCount++;

    host.log(
        "Burst frame " +
        sentCount +
        " of " +
        currentRepeatCount
    );

    if (sentCount >= currentRepeatCount) {
        host.cancelTask(burstTask);
        host.log("Burst completed.");
        host.requestStop();
    }
}

function setup() {
    const currentInterval = numberValue(intervalMs);
    const currentRepeatCount = numberValue(repeatCount);

    if (isNaN(currentInterval) || currentInterval <= 0) {
        host.log("intervalMs must be greater than zero.");
        host.requestStop();
        return;
    }

    if (isNaN(currentRepeatCount) || currentRepeatCount <= 0) {
        host.log("repeatCount must be greater than zero.");
        host.requestStop();
        return;
    }

    sentCount = 0;

    burstTask = host.scheduleEvery(
        currentInterval,
        sendBurstFrame
    );

    if (burstTask === 0) {
        host.log("Unable to schedule burst.");
        host.requestStop();
        return;
    }

    host.log(
        "Starting burst: " +
        currentRepeatCount +
        " frames every " +
        currentInterval +
        " ms."
    );
}