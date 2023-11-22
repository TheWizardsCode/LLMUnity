#pragma once
#include "common.h"
#include "llama.h"

class LLMParser {
    public:
        static bool readline(std::string & line);
        static std::vector<std::string> splitArguments(const std::string& inputString);
        static gpt_params string_to_gpt_params(std::string params_string);
};

class LLM {
    public:
        LLM(gpt_params params);
        LLM(std::string params_string);
        ~LLM();

        void run();

    private:
        gpt_params params;
        llama_model * model;
        std::vector<llama_token> session_tokens;
        std::vector<llama_token> embd_inp;
        std::vector<llama_token> guidance_inp;
        llama_context * ctx;
        int n_ctx;
        int guidance_offset;
        int original_prompt_len;
        bool add_bos;
        struct llama_sampling_context * ctx_sampling;
        llama_context * ctx_guidance;
        std::string path_session;
        size_t n_matching_session_tokens;
        std::vector<llama_token> inp_pfx;
        std::vector<llama_token> inp_sfx;

        void check_params();

        void init();

        void load_model();

        void set_context_num();

        void load_saved_session();

        void init_embedding_input();

        void tokenize_negative_prompt();

        void session_similarity();

        void setup_context();

        void context_swapping(
            std::vector<llama_token>& embd,
            int& n_past,
            int& n_past_guidance
        );

        void reuse_matching_prefix(
            std::vector<llama_token>& embd,
            int& n_session_consumed,
            int& n_past
        );

        void shift_past_guidance(
            std::vector<llama_token>& embd,
            std::vector<llama_token>& embd_guidance,
            int& n_past_guidance,
            int original_prompt_len
        );

        void eval_tokens_in_batches(
            std::vector<llama_token>& embd,
            int& n_past
        );

        void push_prompt_to_sampling_context(
            std::vector<llama_token>& embd,
            bool need_to_save_session,
            int& n_consumed,
            int& n_remain,
            bool& input_echo,
            bool& is_interacting
        );

        void display_text(
            std::vector<llama_token>& embd,
            std::vector<int>& input_tokens,
            std::vector<int>& output_tokens,
            std::ostringstream& output_ss
        );

        bool check_reverse_prompt(
            bool& is_interacting
        );

        std::string get_user_input();

        void add_tokens_to_embd(
            std::string& buffer,
            bool is_antiprompt,
            int& n_consumed,
            std::vector<int>& output_tokens,
            std::ostringstream& output_ss,
            int& n_remain
        );
};

#ifdef _WIN32
    #ifdef MYLIBRARY_EXPORTS
    #define MYLIBRARY_API __declspec(dllexport)
    #else
    #define MYLIBRARY_API __declspec(dllimport)
    #endif
#else
    #define MYLIBRARY_API
#endif

extern "C" {
	MYLIBRARY_API LLM* LLM_Create_Default() { return new LLM(R"(-i -m /home/benuix/codes/llama.cpp/mistral-7b-v0.1.Q4_K_M.gguf -ngl 32 -c 4096 --keep 256 --repeat_penalty 1.1 --prompt "Transcript of a dialog, where the User interacts with an Assistant named Lucy. Lucy is a friendly dinosaur." -r "User:" -s 1234)"); }
	MYLIBRARY_API LLM* LLM_Create(std::string params_string) { return new LLM(params_string); }
	MYLIBRARY_API void LLM_Delete(LLM* object) { delete object; }
	MYLIBRARY_API void LLM_Run(LLM* object) { return object->run(); }
}