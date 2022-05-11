struct Shader { 
unsigned int type;
const char* code;
};
struct ShaderProgram { 
const char* programName;
unsigned int index;
unsigned int numberOfShaders;
};

constexpr unsigned int numberOfShaderPrograms = 1 + 0;
constexpr unsigned int numberOfShaders = 1 + 0;

 ShaderProgram programs[numberOfShaderPrograms] = {
   { "NULL",0,0}
};

Shader shaders[numberOfShaders] = {
   {0,"NULL"}
};
