#pragma once
#include <memory>
#define Ref(x) std::shared_ptr<x>
#define Scope(x) std::unique_ptr<x>
