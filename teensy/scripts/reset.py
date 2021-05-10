Import("env")
import os
from platformio.builder.tools.pioupload import AutodetectUploadPort

platform = env.PioPlatform()

AutodetectUploadPort(env)


# Multiple actions
env.AddCustomTarget(
    name="reset",
    dependencies=None,
    actions=[
        "tycmd reset"
    ],
    title="Reset ESP8266",
    description="Resets the ESP8266 board"
)