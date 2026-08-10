/*
 * SavvyLens scenario scheduler example.
 *
 * Run this script, then press Stop or Stop All at any point.
 * All pending scheduler callbacks are cancelled with the ScriptContainer.
 */

let sequence = 0;
let taskId = 0;

function sendTestFrame() {
    can.sendFrame(
        0,                  // bus
        0x321,              // CAN ID
        8,                  // DLC / byte count
        [
            sequence & 0xFF,
            (sequence >> 8) & 0xFF,
            0xA5,
            0x5A,
            0x00,
            0x00,
            0x00,
            0x00
        ]
    );

    sequence++;

    /*
     * This is intentionally bounded: it proves that a repeating task can
     * cancel itself and that no task remains after the final send.
     */
    if (sequence >= 20) {
        host.cancelTask(taskId);
        host.log("Timed send scenario completed.");
        host.requestStop();
    }
}

function setup() {
    host.log("Starting timed-send scenario.");

    taskId = host.scheduleEvery(1000, sendTestFrame);

    if (taskId === 0) {
        host.log("Unable to create timed-send task.");
    }
}