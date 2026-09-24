#ifndef RAG_BUILDER_MODULE_H
#define RAG_BUILDER_MODULE_H

#define RB_MODULE_ID_MAX       64
#define RB_MODULE_NAME_MAX     64
#define RB_MODULE_PATH_MAX     1024
#define RB_MODULE_SHA256_HEX   65
#define RB_MODULE_MIN_TESTS    10

/*
 * Module ABI
 *
 * 1.2 adds deterministic execution_stage
 * to rb_module_descriptor_t.
 */
#define RB_MODULE_API_MAJOR    1
#define RB_MODULE_API_MINOR    2