/**
 * @file	ThreadPool.h
 * @brief	���񏈗��N���X
 *
 * @date	2021/12/03 2021�N�x����
 * @version	1.00
 * @author	�ђ˗z��
 * @note
 */
#pragma once

#include<mutex>
#include<thread>
#include<vector>
#include<functional>

class Threading
{
public:

	typedef std::function<void()> ThreadClosure;

	class Task
	{
	public:

		Task() = default;
		Task(Threading::ThreadClosure&& threadClosure) noexcept;
		void Run() noexcept { m_threadClosure(); }

	private:

		Threading::ThreadClosure m_threadClosure;
	};

	~Threading();

	bool Start() noexcept;
	void Stop() noexcept;

	void AddTask(ThreadClosure&& function) noexcept;
	bool WaitUntilTheEnd() noexcept;

	bool GetIsRunning() const { return m_isRunning; }

private:

	void Loop() noexcept;

	// ���݁A���񏈗����Ă��邩������
	bool m_isRunning;

	// �Z�b�g���ꂽ�S�Ă�Task
	std::vector<Task> m_tasks;

	// thread�z��
	std::vector<std::thread> m_threads;

	// �f�[�^�����h�~�p
	std::mutex m_mutex;

	// thread�̑҂�
	std::condition_variable m_conditionVar;
};