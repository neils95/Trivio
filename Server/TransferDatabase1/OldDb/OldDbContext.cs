namespace TransferDatabase1.OldDb
{
	using System;
	using System.Data.Entity;
	using System.ComponentModel.DataAnnotations.Schema;
	using System.Linq;

	public partial class OldDbContext : DbContext
	{
		public OldDbContext()
			: base("name=OldDbContext")
		{
		}

		public virtual DbSet<C__MigrationHistory> C__MigrationHistory { get; set; }
		public virtual DbSet<Trivia> Trivias { get; set; }
		public virtual DbSet<User> Users { get; set; }
		public virtual DbSet<UserTriviaHistory> UserTriviaHistories { get; set; }

		protected override void OnModelCreating(DbModelBuilder modelBuilder)
		{
		}
	}
}
