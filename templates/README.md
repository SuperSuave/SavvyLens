# SavvyLens Scenario Templates

SavvyLens scenario templates are editable JavaScript starting points for common CAN-bus experiments. They are ordinary scripts: inserting a template creates an independent copy that can be edited, saved, or reused without changing the original template file.

## Built-in templates

SavvyLens currently ships these built-in templates:

- **Timed Playback**: sends a finite sequence of configured CAN frames at a fixed interval, then stops.
- **On-Frame Response**: listens for a matching CAN frame and transmits a configured response until stopped.
- **Burst / Repeat**: sends one configured frame a fixed number of times, then stops.
- **Byte Sweep**: repeatedly sends a frame while varying one payload byte through a bounded range, then stops.

Finite scenarios use the per-script scheduler and call `host.requestStop()` when complete. Pressing **Stop** or **Stop All** cancels every scheduled task owned by that script before the JavaScript runtime is torn down.

## Using templates

1. Open the **Scripting Interface**.
2. Click **Templates**.
3. Select a built-in or user template.
4. Configure values in the **Public Variables** table.
5. Click **Run**.

Inserting a template creates a new loaded script. Editing or saving that script does not overwrite the template that created it.

## Public Variables

Templates declare configuration values with source comments:

```javascript
// @public bus = 0
// @public id = 0x321
// @public intervalMs = 100
// @public dataText = B4 5A 14 51 00 00 25 05

var bus = 0;
var id = 0x321;
var intervalMs = 100;
var dataText = "B4 5A 14 51 00 00 25 05";
```

SavvyLens parses `// @public name = defaultValue` declarations without running the script. The declared values therefore appear in the **Public Variables** table while the script is stopped.

Users can edit a value before pressing **Run**. SavvyLens evaluates the script, applies the configured values to its JavaScript globals, and then calls `setup()`. This lets `setup()` install filters or schedule work using the values shown in the table.

### Rules for template authors

- Use a top-level `var` for every `@public` parameter.
- The declared manifest name and JavaScript variable name must match exactly.
- Do not use `const` for manifest-backed public variables.
- Values from the Public Variables table are text. Convert numeric settings with `Number(value)` and validate them before use.
- Use a text parameter such as `dataText` for editable byte payloads; parse hexadecimal bytes inside the script.
- Keep internal state such as task IDs, counters, and parsed payload arrays private; do not declare it as `@public`.

## Payload text

Templates use hexadecimal payload text for editable data:

```text
B4 5A 14 51 00 00 25 05
```

Payload parsers accept whitespace- or comma-separated byte values. Every byte must be between `00` and `FF`.

## Scheduler API

The following APIs are available on `host`:

```javascript
const onceId = host.scheduleOnce(500, callback);
const repeatId = host.scheduleEvery(100, callback);

host.cancelTask(onceId);
host.cancelTask(repeatId);
host.requestStop();
```

- `scheduleOnce(delayMs, callback)` schedules one asynchronous callback.
- `scheduleEvery(intervalMs, callback)` schedules a repeating callback.
- `cancelTask(taskId)` cancels only the specified scheduled task; the script remains running.
- `requestStop()` defers a full script stop until the current JavaScript callback has returned safely.

A task ID of `0` indicates that the task could not be created.

## Template locations

Built-in templates are installed with SavvyLens:

```text
<PREFIX>/share/SavvyLens/templates/
```

During development, SavvyLens also looks for a `templates/` folder beside the executable. This supports local Debug and Release runs without a system-wide installation.

User-created templates are stored in:

```text
QStandardPaths::AppDataLocation/templates/
```

The Templates menu refreshes when it opens, so adding a `.js` file to either template directory does not require restarting SavvyLens.

## Creating a user template

1. Start with a built-in template or write a script.
2. Add `// @public` declarations for user-configurable values.
3. Verify the script and its Public Variables.
4. Use **Templates → Save Current Script as Template...**.

The saved script appears under **My Templates** the next time the Templates menu opens.

## Safety behavior

- **Stop** cancels scheduled work, unregisters reactive CAN filters, and tears down the script runtime.
- **Stop All** applies the same safety stop to every loaded script.
- Closing the Scripting Interface stops all scripts.
- A script runtime error cancels that script's scheduled work before teardown.
- Reactive templates such as On-Frame Response intentionally remain running until stopped.
- Finite templates such as Timed Playback, Burst / Repeat, and Byte Sweep stop themselves after completing their configured scenario.