/*
 * Timed CAN playback template.
 *
 * framesText format:
 *   CAN_ID:BYTE BYTE BYTE; CAN_ID:BYTE BYTE BYTE
 *
 * Example:
 *   0x321:B4 5A 14 51 00 00 25 05; 0x322:01 02 03 04
 *
 * intervalMs is read when the scenario starts. Change it and Restart
 * the script to apply a new timer interval.
 */

// @public bus = 0
// @public intervalMs = 100
// @public framesText = 0x321:B4 5A 14 51 00 00 25 05

var bus = 0;
var intervalMs = 100;
var framesText = "0x321:B4 5A 14 51 00 00 25 05";

var playbackTask = 0;
var nextFrame = 0;
var frames = [];

function numberValue(value)
{
    const result = Number(value);
    return isFinite(result) ? result : NaN;
}

function parseHexBytes(text)
{
    const tokens = String(text).trim().split(/[\s,]+/);
    const bytes = [];

    for (let i = 0; i < tokens.length; i++)
    {
        if (tokens[i].length === 0)
        {
            continue;
        }

        const value = parseInt(tokens[i], 16);

        if (isNaN(value) || value < 0 || value > 0xFF)
        {
            return null;
        }

        bytes.push(value);
    }

    return bytes;
}

function parseFrames(text)
{
    const entries = String(text).split(";");
    const parsedFrames = [];

    for (let i = 0; i < entries.length; i++)
    {
        const entry = entries[i].trim();

        if (entry.length === 0)
        {
            continue;
        }

        const separator = entry.indexOf(":");

        if (separator < 1)
        {
            return null;
        }

        const id = numberValue(entry.substring(0, separator).trim());
        const data = parseHexBytes(entry.substring(separator + 1));

        if (isNaN(id) || id < 0 || id > 0x1FFFFFFF ||
            data === null || data.length === 0 || data.length > 64)
        {
            return null;
        }

        parsedFrames.push(
            {
                id: id,
                data: data
            });
    }

    return parsedFrames;
}

function sendNextFrame()
{
    const frame = frames[nextFrame];
    const currentBus = numberValue(bus);

    if (isNaN(currentBus) || currentBus < 0)
    {
        host.log("bus must be zero or greater.");
        host.cancelTask(playbackTask);
        host.requestStop();
        return;
    }

    can.sendFrame(
        currentBus,
        frame.id,
        frame.data.length,
        frame.data
    );

    nextFrame++;

    if (nextFrame >= frames.length)
    {
        host.cancelTask(playbackTask);
        host.log("Timed playback completed.");
        host.requestStop();
    }
}

function setup()
{
    const currentInterval = numberValue(intervalMs);

    if (isNaN(currentInterval) || currentInterval <= 0)
    {
        host.log("intervalMs must be greater than zero.");
        host.requestStop();
        return;
    }

    frames = parseFrames(framesText);

    if (frames === null || frames.length === 0)
    {
        host.log(
            "framesText must use: 0x321:B4 5A 14 51 00 00 25 05"
        );

        host.requestStop();
        return;
    }

    nextFrame = 0;

    playbackTask = host.scheduleEvery(
        currentInterval,
        sendNextFrame
    );

    if (playbackTask === 0)
    {
        host.log("Unable to start timed playback.");
        host.requestStop();
        return;
    }

    host.log(
        "Playing " +
        frames.length +
        " frame(s) every " +
        currentInterval +
        " ms."
    );
}