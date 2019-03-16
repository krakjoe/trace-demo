#ifndef HAVE_PHP_TRACE_DEMO
#define HAVE_PHP_TRACE_DEMO
#include <stdio.h>
#include <curses.h>
#include <php_trace.h>

typedef struct _php_trace_demo_hits_t {
    zend_long hits;
} php_trace_demo_hits_t;

typedef struct _php_trace_demo_line_t {
    zend_long hits;
    zend_long lineno;
} php_trace_demo_line_t;

typedef struct _php_trace_demo_function_t {
    zend_long      hits;
    zend_function *function;
    HashTable      lines;
} php_trace_demo_function_t;

typedef struct _php_trace_demo_context_t {
    zend_bool  started;
    HashTable  maps;
} php_trace_demo_context_t;

static void php_trace_demo_function_free(zval *zv) {
    php_trace_demo_function_t *map = Z_PTR_P(zv);
    
    zend_hash_destroy(&map->lines);
    
    free(map);
}

static void php_trace_demo_instruction_free(zval *zv) {
    free(Z_PTR_P(zv));
}

static int php_trace_demo_hits_sort(const Bucket *a, const Bucket *b) {
    php_trace_demo_hits_t *l = Z_PTR(a->val);
    php_trace_demo_hits_t *r = Z_PTR(b->val);
    
    if (l->hits < r->hits) {
        return 1;
    } else if (l->hits > r->hits) {
        return -1;
    } else {
        return 0;
    }
}

static php_trace_action_result_t php_trace_demo_begin(php_trace_context_t *context, int argc, char **argv) {
    php_trace_demo_context_t *ctx = (php_trace_demo_context_t *) context->ctx;
    
    if (php_trace_begin(context, argc, argv) == PHP_TRACE_OK) {
        initscr();
        
        ctx->started = 1;
                
        zend_hash_init(&ctx->maps, 32, NULL, php_trace_demo_function_free, 1);
        
        return PHP_TRACE_OK;
    }
    
    return PHP_TRACE_QUIT;
}

static php_trace_action_result_t php_trace_demo_frame(php_trace_context_t *context, zend_execute_data *frame, zend_long depth) {
    php_trace_demo_context_t *ctx = (php_trace_demo_context_t*) context->ctx;
    php_trace_demo_function_t *map = zend_hash_index_find_ptr(&ctx->maps, (zend_ulong) frame->func);
    php_trace_demo_line_t *line;
    
    if (!map) {
        php_trace_demo_function_t mapped = {
            .function = frame->func,
            .hits     = 0
        };
        
        zend_hash_init(
            &mapped.lines, 32, NULL, NULL, 1);
        
        map = zend_hash_index_add_mem(&ctx->maps, (zend_ulong) frame->func, &mapped, sizeof(php_trace_demo_function_t));
    }
    
    if (ZEND_USER_CODE(frame->func->type)) {
        line = zend_hash_index_find_ptr(&map->lines, (zend_ulong) frame->opline->lineno);
        
        if (!line) {
            php_trace_demo_line_t lined = {
                .lineno = frame->opline->lineno,
                .hits   = 0
            };
            line = zend_hash_index_add_mem(&map->lines, (zend_ulong) frame->opline->lineno, &lined, sizeof(php_trace_demo_line_t));
        }
        line->hits++;
    }
    
    map->hits++;
    
    zend_hash_sort(&ctx->maps, (compare_func_t) php_trace_demo_hits_sort, 0);
    
    clear();
    
    ZEND_HASH_FOREACH_PTR(&ctx->maps, map) {
        zend_hash_sort(&map->lines, (compare_func_t) php_trace_demo_hits_sort, 0);
        
        if (map->function->common.scope) {
            printw("%s::",
                ZSTR_VAL(map->function->common.scope->name));
        }
        
        printw("%s",
            map->function->common.function_name ?
                ZSTR_VAL(map->function->common.function_name) :
                "main");
                
        if (ZEND_USER_CODE(map->function->type)) {
            php_trace_demo_line_t *line;
            
            printw(" in %s hits: %d\n",
                map->function->op_array.filename ?
                    ZSTR_VAL(map->function->op_array.filename) :
                    "main",
                    map->hits);
                 
            ZEND_HASH_FOREACH_PTR(&map->lines, line) {
                printw("\tline %d hits: %d\n",
                    line->lineno,
                    line->hits);
            } ZEND_HASH_FOREACH_END();   
        } else {
            printw(" <internal> hits: %d\n", map->hits);
        }
        
    } ZEND_HASH_FOREACH_END();
    
    refresh();
    
    return PHP_TRACE_OK;
}

static void php_trace_demo_end(php_trace_context_t *context) {
    php_trace_demo_context_t *ctx = (php_trace_demo_context_t *) context->ctx;
    
    if (ctx->started) {
        zend_hash_destroy(&ctx->maps);
        endwin();
    }
}

int main(int argc, char **argv) {
    int   php_trace_target = atol(argv[1]);
    
    php_trace_demo_context_t php_trace_demo_context = {
        .started = 0
    };
    
    php_trace_context_t php_trace_context = {
        .pid         = php_trace_target,
        .max         = -1,
        .depth       = 1,
        .freq        = 1000,
        .stack       = 0,
        .arData      = 0,
        .strData     = 0,
        .interrupted = 0,

        .onBegin       = php_trace_demo_begin,
        .onAttach      = NULL,
        .onStackStart  = php_trace_stack_start,
        .onFrame       = php_trace_demo_frame,
        .onStackFinish = php_trace_stack_finish,
        .onDetach      = NULL,
        .onEnd         = php_trace_demo_end,
        
        .onSchedule    = php_trace_schedule,
        
        .ctx           = &php_trace_demo_context
    };
    
    return php_trace_loop(&php_trace_context, argc, argv);
}
#endif
