#ifndef PROJECTLABORATORY_FRAMEWORK_H
#define PROJECTLABORATORY_FRAMEWORK_H

#include "src/core/camera.h"
#include "src/core/light.h"
#include "src/core/lightarray.h"
#include "src/core/material.h"
#include "src/core/mesh.h"
#include "src/core/model.h"

#include "src/message/variants/applicationerror.h"
#include "src/message/variants/applicationinfo.h"
#include "src/message/variants/applicationwarning.h"

#include "src/message/applicationmessage.h"
#include "src/message/messagequeue.h"

#include "src/opengl/loader/shaderloader.h"
#include "src/opengl/loader/texture.h"

#include "src/opengl/render/framebuffer.h"
#include "src/opengl/render/framebuffercube.h"
#include "src/opengl/render/rendertarget.h"
#include "src/opengl/render/rendertexture.h"

#include "src/opengl/application.h"
#include "src/opengl/shaderbatch.h"
#include "src/opengl/shaderprogram.h"

#include "src/scene/scene.h"
#include "src/scene/scenebuilder.h"

#include "src/threading/job.h"
#include "src/threading/jobpool.h"
#include "src/threading/threadpool.h"
#include "src/threading/timer.h"

#include "src/uniform/uniform.h"
#include "src/uniform/uniformregistry.h"
#include "src/uniform/uniformsource.h"

#include "src/utility/buffertexture.h"
#include "src/utility/dummytexture.h"
#include "src/utility/mouse.h"

#include "config.h"

#endif //PROJECTLABORATORY_FRAMEWORK_H