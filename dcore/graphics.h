#ifndef DCORE_GRAPHICS_H
#define DCORE_GRAPHICS_H
#include <dcore/common.h>
#include <dcore/math.h>
#include <stddef.h>

typedef struct DCgState DCgState;
typedef struct DCgAlloc DCgAlloc;
typedef struct DCgBuffer DCgBuffer;
typedef struct DCgMaterial DCgMaterial;
typedef void DCgCmdBuffer;
typedef void *DCgCmdPool;

typedef DCgBuffer DCgVertexBuffer;
typedef DCgBuffer DCgIndexBuffer;

/** Creates and allocates a graphics state */
DCgState *dcgNewState();

/**
 * Initializes a graphics state.
 * @param appVersion client version.
 * @param appName client name.
 **/
void dcgInit(DCgState *s, uint32_t appVersion, const char *appName);

/** Deinitializes a graphics state. */
void dcgDeinit(DCgState *s);

/** Frees a graphics state. */
void dcgFreeState(DCgState *state);

/** Returns whether the window should close or not. */
bool dcgShouldClose(DCgState *state);

/** Closes the window. (shouldClose flag is set) */
void dcgClose(DCgState *state);

/** Retries the position of the mouse relative to the window. */
void dcgGetMousePosition(DCgState *state, DCmVector2i mousePosition);

/** Updates the window. (polls for new events) */
void dcgUpdate(DCgState *state);

typedef enum DCgQueueFamilyType {
	DCG_CMD_POOL_TYPE_GRAPHICS,
	DCG_CMD_POOL_TYPE_COMPUTE,
} DCgCmdPoolType;

DCgCmdPool *dcgNewCmdPool(DCgState *s, DCgCmdPoolType type);
void dcgFreeCmdPool(DCgState *s, DCgCmdPool *pool);

/** Creates a new command buffer */
DCgCmdBuffer *dcgGetNewCmdBuffer(DCgState *s, DCgCmdPool *pool);

/** Begin command buffer. */
void dcgCmdBegin(DCgState *s, DCgCmdBuffer *cmds);

/** End command buffer. */
void dcgCmdEnd(DCgState *s, DCgCmdBuffer *cmds);

/** Binds a vertex buffer. */
void dcgCmdBindVertexBuf(DCgState *s, DCgCmdBuffer *cmds, const DCgVertexBuffer *vbuf);

enum DCgIndexType {
	DCG_INDEX_TYPE_UINT16 = 0,
	DCG_INDEX_TYPE_UINT32 = 1,
};

/** Binds an index buffer. */
void dcgCmdBindIndexBuf(DCgState *s, DCgCmdBuffer *cmds, const DCgIndexBuffer *ibuf, enum DCgIndexType type);

/** Binds a material (pipelines + stuff). */
void dcgCmdBindMat(DCgState *s, DCgCmdBuffer *cmds, DCgMaterial *mat);

typedef struct DCgCmdBeginRenderPassInfo {
	size_t renderPassIndex;
	DCmRect2 renderArea;
	size_t clearValueCount;
	const struct {
		DCmVector4f color;
		union {
			float depth;
			uint32_t stencil;
		} depthStencil;
	} * clearValues;
} DCgCmdBeginRenderPassInfo;

/** Begins a render pass with index `index`. */
void dcgCmdBeginRenderPass(DCgState *s, DCgCmdBuffer *cmds, const DCgCmdBeginRenderPassInfo *info);

/** Ends the current render pass. */
void dcgCmdEndRenderPass(DCgState *s, DCgCmdBuffer *cmds);

/** Draws some instances with the specified
 * number indices from the bound vertex/index buffers.
 * @param indices number of indices to draw per instance.
 * @param instances number of instances to draw. */
void dcgCmdDraw(DCgState *s, DCgCmdBuffer *cmds, size_t indices, size_t instances);

/** Submit a command buffer into a queue. */
void dcgSubmit(DCgState *s, DCgCmdBuffer *cmds, int queue);

typedef enum DCgPipelineStage {
	DCG_SHADER_STAGE_VERTEX = 0x01,
	DCG_SHADER_STAGE_GEOMETRY = 0x08,
	DCG_SHADER_STAGE_FRAGMENT = 0x10,
	DCG_SHADER_STAGE_COMPUTE = 0x20,
} DCgShaderStage;

typedef struct DCgShaderModule {
	DCgShaderStage stage;
	void *module;
	const char *name;
} DCgShaderModule;

typedef enum DCgCullMode { DCG_CULL_MODE_NONE, DCG_CULL_MODE_FRONT, DCG_CULL_MODE_BACK, DCG_CULL_MODE_BOTH } DCgCullMode;

typedef enum DCgPolygonMode {
	DCG_POLYGON_MODE_FILL,
	DCG_POLYGON_MODE_LINE,
	DCG_POLYGON_MODE_POINT,
} DCgPolygonMode;

typedef enum DCgCompareOp {
	DCG_COMAPRE_OP_NEVER,
	DCG_COMAPRE_OP_LESS,
	DCG_COMAPRE_OP_EQUAL,
	DCG_COMAPRE_OP_LESS_EQUAL,
	DCG_COMAPRE_OP_GREATER,
	DCG_COMAPRE_OP_NOT_EQUAL,
	DCG_COMAPRE_OP_GREATER_OR_EQUAL,
	DCG_COMAPRE_OP_ALWAYS,
} DCgCompareOp;

typedef struct DCgMaterialOptions {
	DCmOffset2 scissorOffset;
	DCmExtent2 scissorExtent;
	DCgCullMode cullMode;
	float lineWidth;
	DCgPolygonMode polygonMode;
	bool enableDiscard;
	bool enableDepthTest;
	bool enableDepthWrite;
	DCgCompareOp depthCompareOp;
	bool enableDepthBoundsTest;
	float minDepthBound, maxDepthBound;
	bool enableStencilTest;
	DCmExtent2 viewportExtent;
	int pushConstantsIndex, descriptorSetsIndex, vertexInputIndex;
} DCgMaterialOptions;

typedef struct DCgMaterialCache DCgMaterialCache;

DCgMaterial *dcgNewMaterial(
  DCgState *state, size_t moduleCount, DCgShaderModule *modules, const DCgMaterialOptions *options, const DCgMaterialCache *cache /* = NULL */
);

DCgMaterialCache *dcgGetMaterialCache(DCgState *state, DCgMaterial *material);
void dcgFreeMaterial(DCgState *state, DCgMaterial *material);

void dcgInitShaderModule(DCgState *state, DCgShaderModule *module, DCgShaderStage stage, size_t size, uint32_t *code, const char *name);
void dcgFreeShaderModule(DCgState *state, DCgShaderModule *module);

#endif
