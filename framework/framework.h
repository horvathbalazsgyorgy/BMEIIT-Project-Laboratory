#ifndef PROJECTLABORATORY_FRAMEWORK_H
#define PROJECTLABORATORY_FRAMEWORK_H

#include "core/camera.h"
#include "core/light.h"
#include "core/lightarray.h"
#include "core/material.h"
#include "core/mesh.h"
#include "core/model.h"

#include "message/variants/applicationerror.h"
#include "message/variants/applicationinfo.h"
#include "message/variants/applicationwarning.h"

#include "message/applicationmessage.h"
#include "message/messagequeue.h"

#include "opengl/loader/shaderloader.h"
#include "opengl/loader/texture.h"

#include "opengl/render/framebuffer.h"
#include "opengl/render/framebuffercube.h"
#include "opengl/render/rendertarget.h"
#include "opengl/render/rendertexture.h"

#include "opengl/application.h"
#include "opengl/shaderbatch.h"
#include "opengl/shaderprogram.h"

#include "scene/scene.h"
#include "scene/scenebuilder.h"

#include "threading/job.h"
#include "threading/jobpool.h"
#include "threading/threadpool.h"
#include "threading/timer.h"

#include "uniform/uniform.h"
#include "uniform/uniformregistry.h"
#include "uniform/uniformsource.h"

#include "utility/buffertexture.h"
#include "utility/dummytexture.h"
#include "utility/mouse.h"

#endif //PROJECTLABORATORY_FRAMEWORK_H