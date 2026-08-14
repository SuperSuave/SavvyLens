/*
 * On-frame CAN response template.
 *
 * Listens for requestId on requestBus and sends responseDataText when a
 * matching frame is received. It remains Running until Stop or Stop All.
 *
 * responseDataText accepts hexadecimal bytes separated by spaces or commas.
 */

// @public requestBus = 0
// @public requestId = 0x321
// @public requestMask = 0x7FF
// @public responseBus = 0
// @public responseId = 0x322
// @public responseDataText = B4 5A 14 51 00 00 25 05

var requestBus = 0;
var requestId = 0x321;
var requestMask = 0x7FF;

var responseBus = 0;
var responseId = 0x322;

var responseDataText = "B4 5A 14 51 00 00 25 05";

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

function setup() {
    const currentRequestBus = numberValue(requestBus);
    const currentRequestId = numberValue(requestId);
    const currentRequestMask = numberValue(requestMask);

    if (isNaN(currentRequestBus) || currentRequestBus < 0 ||
        isNaN(currentRequestId) || currentRequestId < 0 ||
        currentRequestId > 0x1FFFFFFF ||
        isNaN(currentRequestMask) || currentRequestMask < 0) {
        host.log("Invalid request bus, ID, or mask.");
        host.requestStop();
        return;
    }

    can.setFilter(
        currentRequestId,
        currentRequestMask,
        currentRequestBus
    );

    host.log(
        "Waiting for CAN ID 0x" +
        currentRequestId.toString(16).toUpperCase()
    );
}

function gotCANFrame(bus, id, length, data) {
    const currentResponseBus = numberValue(responseBus);
    const currentResponseId = numberValue(responseId);
    const responseData = parseHexBytes(responseDataText);

    if (isNaN(currentResponseBus) || currentResponseBus < 0 ||
        isNaN(currentResponseId) || currentResponseId < 0 ||
        currentResponseId > 0x1FFFFFFF) {
        host.log("Invalid response bus or ID.");
        return;
    }

    if (responseData === null ||
        responseData.length === 0 ||
        responseData.length > 64) {
        host.log("responseDataText must contain valid hexadecimal bytes.");
        return;
    }

    can.sendFrame(
        currentResponseBus,
        currentResponseId,
        responseData.length,
        responseData
    );

    host.log(
        "Responded to CAN ID 0x" +
        id.toString(16).toUpperCase()
    );
}