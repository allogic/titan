#include <ti_pch.h>
#include <ti_clang.h>

// TODO: Check all VkImageMemoryBarrier's and remove double transitions depending on current renderpass (VkAttachmentDescription)
// TODO: Rename functions with their proper module name..

// TODO
/*

I reviewed [eg_clang.cpp](C:/Users/Brunzhilde/Downloads/eg_clang.cpp). There are several concrete problems.

[P1] Some calls do not match Titan’s current Clang headers

At [line 93 (line 93)](C:/Users/Brunzhilde/Downloads/eg_clang.cpp:93):

createFileManager() returns void.

createSourceManager() takes no arguments.

getVirtualFile() has been replaced by getVirtualFileRef().

Against your current headers, the initialization needs this form:

s_compiler_instance->createFileManager();
s_compiler_file_manager = &s_compiler_instance->getFileManager();

s_compiler_instance->createSourceManager();
s_compiler_source_manager = &s_compiler_instance->getSourceManager();

The virtual-file call at line 106 also needs updating.

[P1] Diagnostic objects have conflicting owners

At [lines 65–67 (line 65)](C:/Users/Brunzhilde/Downloads/eg_clang.cpp:65), two different ownership problems exist:

make_shared owns the diagnostic printer, while the final true tells DiagnosticsEngine to delete it too.

DiagnosticIDs and DiagnosticsEngine use Clang’s intrusive reference counting internally, but your globals manage them independently with std::shared_ptr.

This can cause invalid deletion during shutdown.

Fix: use llvm::IntrusiveRefCntPtr for the IDs and diagnostics engine. If you keep the printer’s existing owner, pass false for ShouldOwnClient.

[P1] Repeated compilation retains a freed source buffer

At [line 104 (line 104)](C:/Users/Brunzhilde/Downloads/eg_clang.cpp:104), addRemappedFile() stores a raw pointer. Your local memory_buffer is destroyed when eg_clang_compile() returns, but the persistent invocation retains that pointer.

On the next compilation, Clang processes the old entry and accesses freed memory.

Fix: clear previous remappings before adding the current buffer. Keep that buffer alive throughout compilation. RetainRemappedFileBuffers = true leaves ownership with you; it does not copy the buffer.
[P1] Compilation continues after a compiler error

At [line 119 (line 119)](C:/Users/Brunzhilde/Downloads/eg_clang.cpp:119), failure only prints a message. Execution continues to:

module->getDataLayout()

However, takeModule() can return null after failure.

Fix: return when ExecuteAction() fails, and check module before using it. A syntax error should end this compilation attempt.

[P1] Valid C++ input can fail the "add" lookup

You select C++ at line 72, but [line 142 (line 142)](C:/Users/Brunzhilde/Downloads/eg_clang.cpp:142) looks up the literal symbol "add".

This ordinary C++ function receives a decorated symbol name:

int add(int a, int b) {
  return a + b;
}

For the existing lookup, the compiled source needs C linkage:

extern "C" int add(int a, int b) {
  return a + b;
}

Also, cantFail(jit->lookup("add")) makes a missing function fatal. Handle lookup failure before calling the function pointer.

[P2] ThreadSafeModule receives the wrong context

At [line 138 (line 138)](C:/Users/Brunzhilde/Downloads/eg_clang.cpp:138), you create a new context unrelated to the generated module. The wrapper therefore manages and locks the wrong context. LLVM’s ownership explanation.

Use the context belonging to the action:

llvm::orc::ThreadSafeModule thread_safe_module(
  std::move(module),
  std::unique_ptr<llvm::LLVMContext>(emit_llvm_only_action.takeLLVMContext()));
[P2] Expected results are accessed without checking

This happens with:

file_entry_ref at line 110.

target_machine_builder at line 133.

symbol at line 146.

LLVM builds with its checked-error support can abort even when an unchecked result contains a value. Failed results also cannot safely be dereferenced.

At line 142, cantFail() already extracts an ExecutorAddr; wrapping it back into Expected<ExecutorAddr> creates another unchecked result unnecessarily.

Verification: reviewed against your local Clang/LLVM headers and official LLVM source. I have not compiled or executed this file, and made no changes.

*/

static void import_dflt_assets(void);
static void create_dflt_assets(void);

int32_t main(int32_t argc, char **argv) {
  dmalloc_init();
  
  __try {

    fs_create(ROOT_DIR "/static", ROOT_DIR "/asset");
    ti_clang_create();
    scene_create(&g_scene, "test", "asset/scene/test.pak");

    import_dflt_assets();
    create_dflt_assets();

    pl_window_create(&g_pl_window);
    pl_window_run(&g_pl_window);
    pl_window_destroy(&g_pl_window);

    scene_destroy(&g_scene);
    ti_clang_destroy();
    fs_destroy();

  } __except (EXCEPTION_EXECUTE_HANDLER) {

    printf("Something went wrong..\n"); // TODO
  }

  dmalloc_cleanup();

  return 0;
}

static void import_dflt_assets(void) {
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/veigar_greybeard.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/veigar_greybeard.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/viktor.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/viktor.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/demonblade_tryndamere.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/demonblade_tryndamere.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/spirit_blossom_springs_teemo.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/spirit_blossom_springs_teemo.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/prestige_pandemonium_shaco.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/prestige_pandemonium_shaco.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/old_god_malphite.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/old_god_malphite.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/high_noon_locke.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/high_noon_locke.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/immortalized_legend_kaisa.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/immortalized_legend_kaisa.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/old_god_mordekaiser.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/old_god_mordekaiser.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/steel_legion_garen.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/steel_legion_garen.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/pulsefire_caitlyn.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/pulsefire_caitlyn.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/grand_reckoning_sion.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/grand_reckoning_sion.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/marauder_kalista.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/marauder_kalista.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }

  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_PIPELINE,
      .path = "asset/pipeline/standard_brdf/main.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_pipeline(&asset, FS_PIPELINE_TYPE_DEFAULT,
                             "static/shader/standard_brdf/main.vert",
                             "static/shader/standard_brdf/main.frag",
                             "", "", "", "", "", "", "") == 0) {

        fs_pipeline_t *pipeline = (fs_pipeline_t *)asset.instance;

        pipeline->pipeline_type = FS_PIPELINE_TYPE_DEFAULT;
        pipeline->primitive_topology_index = vk_find_primitive_topology_index(VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
        pipeline->polygon_mode_index = vk_find_polygon_mode_index(VK_POLYGON_MODE_FILL);
        pipeline->cull_mode_flags = VK_CULL_MODE_BACK_BIT;
        pipeline->enable_blending = 1;
        pipeline->enable_depth_test = 1;
        pipeline->enable_depth_write = 1;
        pipeline->descriptor_set_count = 1;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_PIPELINE,
      .path = "asset/pipeline/debug_line/main.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_pipeline(&asset, FS_PIPELINE_TYPE_DEFAULT,
                             "static/shader/debug_line/main.vert",
                             "static/shader/debug_line/main.frag",
                             "", "", "", "", "", "", "") == 0) {

        fs_pipeline_t *pipeline = (fs_pipeline_t *)asset.instance;

        pipeline->pipeline_type = FS_PIPELINE_TYPE_DEFAULT;
        pipeline->primitive_topology_index = vk_find_primitive_topology_index(VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
        pipeline->polygon_mode_index = vk_find_polygon_mode_index(VK_POLYGON_MODE_FILL);
        pipeline->cull_mode_flags = VK_CULL_MODE_BACK_BIT;
        pipeline->enable_blending = 1;
        pipeline->enable_depth_test = 1;
        pipeline->enable_depth_write = 1;
        pipeline->descriptor_set_count = 1;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }

  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_FONT,
      .path = "asset/font/commit_mono_latin_400_normal.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_font(&asset, "static/font/commit_mono_latin_400_normal.ttf") == 0) {

        fs_font_t *font = (fs_font_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_FONT,
      .path = "asset/font/material_symbols_rounded_fill.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_font(&asset, "static/font/material_symbols_rounded_fill.ttf") == 0) {

        fs_font_t *font = (fs_font_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
}
static void create_dflt_assets(void) {
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_SWAPCHAIN,
      .path = "asset/swapchain/main.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_swapchain_t *swapchain = (fs_swapchain_t *)asset.instance;

      swapchain->image_count = 2;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_RENDERER,
      .path = "asset/renderer/main.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_renderer_t *renderer = (fs_renderer_t *)asset.instance;

      strcpy(renderer->debug_line_vertex_buffer.reference_path, "asset/renderer/main/buffer/debug_line_vertex.pak");
      strcpy(renderer->debug_line_index_buffer.reference_path, "asset/renderer/main/buffer/debug_line_index.pak");
      strcpy(renderer->full_screen_vertex_buffer.reference_path, "asset/renderer/main/buffer/full_screen_vertex.pak");
      strcpy(renderer->full_screen_index_buffer.reference_path, "asset/renderer/main/buffer/full_screen_index.pak");

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }

  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_RENDERPASS,
      .path = "asset/renderpass/main.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_renderpass_t *renderpass = (fs_renderpass_t *)asset.instance;

      renderpass->initial_color_attachment_layout_index = vk_find_image_layout_index(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
      renderpass->initial_depth_attachment_layout_index = vk_find_image_layout_index(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
      renderpass->final_color_attachment_layout_index = vk_find_image_layout_index(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
      renderpass->final_depth_attachment_layout_index = vk_find_image_layout_index(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_RENDERPASS,
      .path = "asset/renderpass/imgui.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_renderpass_t *renderpass = (fs_renderpass_t *)asset.instance;

      renderpass->initial_color_attachment_layout_index = vk_find_image_layout_index(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
      renderpass->initial_depth_attachment_layout_index = vk_find_image_layout_index(VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL);
      renderpass->final_color_attachment_layout_index = vk_find_image_layout_index(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
      renderpass->final_depth_attachment_layout_index = vk_find_image_layout_index(VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL);

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }

  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_FRAMEBUFFER,
      .path = "asset/framebuffer/main.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_framebuffer_t *framebuffer = (fs_framebuffer_t *)asset.instance;

      framebuffer->color_attachment_count = 1;
      framebuffer->color_attachments = (fs_asset_reference_t *)TI_ALLOC(sizeof(fs_asset_reference_t), 1, 0);

      strcpy(framebuffer->color_attachments[0].reference_path, "asset/framebuffer/main/attachment/color.pak");
      strcpy(framebuffer->depth_attachment.reference_path, "asset/framebuffer/main/attachment/depth.pak");

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_FRAMEBUFFER,
      .path = "asset/framebuffer/imgui.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_framebuffer_t *framebuffer = (fs_framebuffer_t *)asset.instance;

      framebuffer->color_attachment_count = 1;
      framebuffer->color_attachments = (fs_asset_reference_t *)TI_ALLOC(sizeof(fs_asset_reference_t), 1, 0);

      strcpy(framebuffer->color_attachments[0].reference_path, "asset/framebuffer/imgui/attachment/color.pak");
      strcpy(framebuffer->depth_attachment.reference_path, "asset/framebuffer/imgui/attachment/depth.pak");

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }

  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_BUFFER,
      .path = "asset/buffer/time_info.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_buffer_t *buffer = (fs_buffer_t *)asset.instance;

      buffer->zero_data = 0;
      buffer->size = sizeof(vk_time_info_t);
      buffer->buffer_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      buffer->memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      buffer->memory_allocate_flags = 0;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_BUFFER,
      .path = "asset/buffer/screen_info.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_buffer_t *buffer = (fs_buffer_t *)asset.instance;

      buffer->zero_data = 0;
      buffer->size = sizeof(vk_screen_info_t);
      buffer->buffer_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      buffer->memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      buffer->memory_allocate_flags = 0;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_BUFFER,
      .path = "asset/buffer/mouse_info.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_buffer_t *buffer = (fs_buffer_t *)asset.instance;

      buffer->zero_data = 0;
      buffer->size = sizeof(vk_mouse_info_t);
      buffer->buffer_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      buffer->memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      buffer->memory_allocate_flags = 0;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_BUFFER,
      .path = "asset/buffer/camera_info.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_buffer_t *buffer = (fs_buffer_t *)asset.instance;

      buffer->zero_data = 0;
      buffer->size = sizeof(vk_camera_info_t);
      buffer->buffer_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      buffer->memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      buffer->memory_allocate_flags = 0;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }

  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_IMAGE,
      .path = "asset/framebuffer/main/attachment/color.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_image_t *image = (fs_image_t *)asset.instance;

      image->width = 0;  // TODO
      image->height = 0; // TODO
      image->depth = 1;  // TODO
      image->mip_levels = 1;
      image->format_index = vk_find_format_index(VK_FORMAT_R8G8B8A8_UNORM);
      image->image_layout_index = vk_find_image_layout_index(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
      image->image_type_index = vk_find_image_type_index(VK_IMAGE_TYPE_2D);
      image->image_tiling_index = vk_find_image_tiling_index(VK_IMAGE_TILING_OPTIMAL);
      image->image_view_type_index = vk_find_image_view_type_index(VK_IMAGE_VIEW_TYPE_2D);
      image->image_usage_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
      image->image_aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT;
      image->memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
      image->memory_allocate_flags = 0;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_IMAGE,
      .path = "asset/framebuffer/main/attachment/depth.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_image_t *image = (fs_image_t *)asset.instance;

      image->width = 0;  // TODO
      image->height = 0; // TODO
      image->depth = 1;  // TODO
      image->mip_levels = 1;
      image->format_index = vk_find_format_index(VK_FORMAT_D32_SFLOAT);
      image->image_layout_index = vk_find_image_layout_index(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
      image->image_type_index = vk_find_image_type_index(VK_IMAGE_TYPE_2D);
      image->image_tiling_index = vk_find_image_tiling_index(VK_IMAGE_TILING_OPTIMAL);
      image->image_view_type_index = vk_find_image_view_type_index(VK_IMAGE_VIEW_TYPE_2D);
      image->image_usage_flags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
      image->image_aspect_flags = VK_IMAGE_ASPECT_DEPTH_BIT;
      image->memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
      image->memory_allocate_flags = 0;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }

  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_IMAGE,
      .path = "asset/framebuffer/imgui/attachment/color.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_image_t *image = (fs_image_t *)asset.instance;

      image->width = 0;  // TODO
      image->height = 0; // TODO
      image->depth = 1;  // TODO
      image->mip_levels = 1;
      image->format_index = vk_find_format_index(VK_FORMAT_R8G8B8A8_UNORM);
      image->image_layout_index = vk_find_image_layout_index(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
      image->image_type_index = vk_find_image_type_index(VK_IMAGE_TYPE_2D);
      image->image_tiling_index = vk_find_image_tiling_index(VK_IMAGE_TILING_OPTIMAL);
      image->image_view_type_index = vk_find_image_view_type_index(VK_IMAGE_VIEW_TYPE_2D);
      image->image_usage_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
      image->image_aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT;
      image->memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
      image->memory_allocate_flags = 0;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_IMAGE,
      .path = "asset/framebuffer/imgui/attachment/depth.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_image_t *image = (fs_image_t *)asset.instance;

      image->width = 0;  // TODO
      image->height = 0; // TODO
      image->depth = 1;  // TODO
      image->mip_levels = 1;
      image->format_index = vk_find_format_index(VK_FORMAT_D32_SFLOAT);
      image->image_layout_index = vk_find_image_layout_index(VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL);
      image->image_type_index = vk_find_image_type_index(VK_IMAGE_TYPE_2D);
      image->image_tiling_index = vk_find_image_tiling_index(VK_IMAGE_TILING_OPTIMAL);
      image->image_view_type_index = vk_find_image_view_type_index(VK_IMAGE_VIEW_TYPE_2D);
      image->image_usage_flags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
      image->image_aspect_flags = VK_IMAGE_ASPECT_DEPTH_BIT;
      image->memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
      image->memory_allocate_flags = 0;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }

  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_BUFFER,
      .path = "asset/renderer/main/buffer/debug_line_vertex.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_buffer_t *buffer = (fs_buffer_t *)asset.instance;

      buffer->zero_data = 0;
      buffer->size = sizeof(vk_debug_line_vertex_t) * TI_DEBUG_LINE_VERTEX_COUNT;
      buffer->buffer_usage_flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      buffer->memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      buffer->memory_allocate_flags = 0;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_BUFFER,
      .path = "asset/renderer/main/buffer/debug_line_index.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_buffer_t *buffer = (fs_buffer_t *)asset.instance;

      buffer->zero_data = 0;
      buffer->size = sizeof(vk_debug_line_index_t) * TI_DEBUG_LINE_INDEX_COUNT;
      buffer->buffer_usage_flags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      buffer->memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      buffer->memory_allocate_flags = 0;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_BUFFER,
      .path = "asset/renderer/main/buffer/full_screen_vertex.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_buffer_t *buffer = (fs_buffer_t *)asset.instance;

      buffer->zero_data = 0;
      buffer->size = sizeof(vk_full_screen_vertex_t) * 4;
      buffer->buffer_usage_flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      buffer->memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      buffer->memory_allocate_flags = 0;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_BUFFER,
      .path = "asset/renderer/main/buffer/full_screen_index.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_buffer_t *buffer = (fs_buffer_t *)asset.instance;

      buffer->zero_data = 0;
      buffer->size = sizeof(vk_full_screen_index_t) * 6;
      buffer->buffer_usage_flags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      buffer->memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      buffer->memory_allocate_flags = 0;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }
}
