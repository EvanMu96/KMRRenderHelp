#include "../RenderHelp/RenderHelp.h"
#include "../RenderHelp/Model.h"

int main() {
	RenderHelp rh(600, 800);
	
	Model model("../resources/diablo3_pose.obj");

	Vec3f eye_pos = {0, -0.5, 1.7};
	Vec3f eye_at = {0, 0, 0};
	Vec3f eye_up = {0, 1, 0};
	Vec3f light_dir = {1, 1, 0.85};			// ��ķ���
	float perspective = 3.1415926f * 0.5f;

	Mat4x4f mat_model = matrix_set_scale(1, 1, 1);
	Mat4x4f mat_view = matrix_set_lookat(eye_pos, eye_at, eye_up);
	Mat4x4f mat_proj = matrix_set_perspective(perspective, 6 / 8.0, 1.0, 500.0f);
	Mat4x4f mat_mvp = mat_model * mat_view * mat_proj;

	Mat4x4f mat_model_it = matrix_invert(mat_model).Transpose();

	struct { Vec3f pos; Vec3f normal; Vec2f uv; } vs_input[3];

	const int VARYING_UV = 0;
	const int VARYING_EYE = 1;

	rh.SetVertexShader([&] (int index, ShaderContext& output) -> Vec4f {
			Vec4f pos = vs_input[index].pos.xyz1() * mat_mvp;
			// ������λ�ô�ģ�Ϳռ�ת��Ϊ��������ϵ
			Vec3f pos_world = (vs_input[index].pos.xyz1() * mat_model).xyz();
			// ����ģ�Ͷ��㵽�۾��ķ���
			Vec3f eye_dir = eye_pos - pos_world;
			output.varying_vec2f[VARYING_UV] = vs_input[index].uv;
			output.varying_vec3f[VARYING_EYE] = eye_dir;
			return pos;
		});

	rh.SetPixelShader([&] (ShaderContext& input) -> Vec4f {
			Vec2f uv = input.varying_vec2f[VARYING_UV];
			// ģ���ϵ�ǰ�㵽�۾��ķ���
			Vec3f eye_dir = input.varying_vec3f[VARYING_EYE];
			// ��һ�����շ���
			Vec3f l = vector_normalize(light_dir);
			// ������ͼȡ������ת��Ϊ��������ϵ
			Vec3f n = (model.normal(uv).xyz1() * mat_model_it).xyz();
			// ��ģ����ȡ����ǰ��ĸ߹����
			float s = model.Specular(uv);
			// ���㷴�����
			Vec3f r = vector_normalize(n * vector_dot(n, l) * 2.0f - l);
			// ����߹�
			float p = Saturate(vector_dot(r, eye_dir));
			float spec = Saturate(pow(p, s * 20) * 0.05);
			// �ۺϹ���ǿ��
			float intense = Saturate(vector_dot(n, l)) + 0.2f + spec;
			Vec4f color = model.diffuse(uv);
			return color * intense;
		});


	for (int i = 0; i < model.nfaces(); i++) {
		for (int j = 0; j < 3; j++) {
			vs_input[j].pos = model.vert(i, j);
			vs_input[j].uv = model.uv(i, j);
			vs_input[j].normal = model.normal(i, j);
		}
		rh.DrawPrimitive();
	}

	rh.SaveFile("output.bmp");

#if defined(WIN32) || defined(_WIN32)
	system("mspaint output.bmp");
#endif

	return 0;

}