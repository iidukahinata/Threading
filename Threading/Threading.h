/**
 * @file	ThreadPool.h
 * @brief	並列処理クラス
 *
 * @date	2021/12/03 2021年度初版
 * @version	1.00
 * @author	飯塚陽太
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

	// 現在、並列処理しているかを持つ
	bool m_isRunning;

	// セットされた全てのTask
	std::vector<Task> m_tasks;

	// thread配列
	std::vector<std::thread> m_threads;

	// データ競合防止用
	std::mutex m_mutex;

	// threadの待ち
	std::condition_variable m_conditionVar;
};