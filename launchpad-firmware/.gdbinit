target extended-remote :3333
#monitor reset init

#enable prettyprinting
set print pretty


define reset
monitor reset init
end
document reset
Resets target by issuing a `monitor reset init` command.
end
