#pragma once

bool VMRequireFile(const char *filename, VM *vm, VMError *errorsOut);
void AddVMBindings(VM *vm);
void SandboxVM(VM *vm);
void AddJSONBindings(VM *vm);
